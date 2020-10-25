#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

RegisterDialog::RegisterDialog(const ProxyInfo& proxy, const QString& host, int port, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog),
	proxy_(proxy)
{
    ui->setupUi(this);
	ui->code_group->setVisible(false);
    connect(&register_websocket_, &QWebSocket::connected, this, &RegisterDialog::OnConnected);
    connect(&register_websocket_, &QWebSocket::disconnected, this, &RegisterDialog::OnDisconnect);
	register_websocket_.open(QUrl(QString("ws://%1:%2").arg(host).arg(port)));
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::OnConnected(){
    connect(&register_websocket_, &QWebSocket::textMessageReceived,
            this, &RegisterDialog::onMessageReceived);
}

void RegisterDialog::OnDisconnect(){
	//QMessageBox::warning(this, QString::fromStdWString(L"错误"), QString::fromStdWString(L"连接服务器失败"));
}


void RegisterDialog::onMessageReceived(const QString& message){
	if (register_status_ == kStatusInit)
	{
		ui->tips->setText(message);
		return;
	}
	QJsonParseError jsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
	if (!doucment.isObject())
	{
		return;
	}
	QJsonObject root = doucment.object();
	QString result = root["result"].toString();
	if (result.isEmpty())
	{
		ui->tips->setText(QString::fromStdWString(L"网络可能不通，也许需要设置代理"));
		return;
	}
	result = QByteArray::fromBase64(result.toUtf8());

    switch (register_status_) {
    case kStatusExist:
    {
		HandleExistStatus(result);
    }
        break;
    case kStatusCodeRequest:
    {
		HandleCoderequestStatus(result);
    }
        break;
    case kStatusReigster:
    {
		HandleRegisterStatus(result);
    }
        break;
    default:
        break;
    }
}

void RegisterDialog::on_coderequest_clicked()
{
    QString cc = ui->cc->text();
    QString phone = ui->phone->text();
    if(cc.isEmpty() || phone.isEmpty()){
        QMessageBox::warning(this,QString::fromStdWString(L"错误"), QString::fromStdWString(L"参数没填完整"));
        return;
    }
	HandleInitStatus();
}

void RegisterDialog::on_register_2_clicked()
{
	QString cc = ui->cc->text();
	QString phone = ui->phone->text();
	QString code = ui->code->text();
	if (cc.isEmpty() || phone.isEmpty() || code.isEmpty()){
		QMessageBox::warning(this, QString::fromStdWString(L"错误"), QString::fromStdWString(L"参数没填完整"));
		return;
	}

	QJsonObject command;
	command.insert("task_id", current_task_id_++);
	command.insert("type", "register");

	command.insert("proxytype", "socks5");
	command.insert("proxy_host", proxy_.proxy_host);
	command.insert("proxy_port", proxy_.proxy_port);
	command.insert("username", proxy_.proxy_username);
	command.insert("password", proxy_.proxy_password);
	command.insert("cc", ui->cc->text());
	command.insert("phone", ui->phone->text());

	command.insert("code", ui->code->text());

	QJsonDocument document;
	document.setObject(command);
	QByteArray byteArray = document.toJson(QJsonDocument::Compact);
	register_websocket_.sendTextMessage(QString(byteArray));
	register_status_ = kStatusReigster;
}

void RegisterDialog::HandleInitStatus(){
    //发送获取账号状态请求
	QJsonObject command;
	command.insert("task_id", current_task_id_++);
	command.insert("type", "exists");


	command.insert("proxytype", "socks5");
	command.insert("proxy_host", proxy_.proxy_host);
	command.insert("proxy_port", proxy_.proxy_port);
	command.insert("username", proxy_.proxy_username);
	command.insert("password", proxy_.proxy_password);
	command.insert("cc", ui->cc->text());
	command.insert("phone", ui->phone->text());

	command.insert("os_ver", "7.0");
	command.insert("manufacture", "samsung");
	command.insert("device_name", "G9208");
	command.insert("build_version", "samsung-userdebug 4.4.4 KTU84P eng.se.infra.20170110.154925 release-keys");


	QJsonDocument document;
	document.setObject(command);
	QByteArray byteArray = document.toJson(QJsonDocument::Compact);
	register_websocket_.sendTextMessage(QString(byteArray));
	register_status_ = kStatusExist;
	ui->tips->setText(QString::fromStdWString(L"获取账号是否存在..."));
}

void RegisterDialog::HandleExistStatus(const QString& message){
	QJsonParseError jsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
	if (!doucment.isObject())
	{
		return;
	}
	QJsonObject root = doucment.object();
	QString status = root["status"].toString();
	if (status == "ok") {
		ui->tips->setText(QString::fromStdWString(L"账号存在可以直接登录"));
	}
	else
	{
		//发送获取验证码请求
		ui->tips->setText(QString::fromStdWString(L"正在获取手机验证码..."));
		QJsonObject command;
		command.insert("task_id", current_task_id_++);
		command.insert("type", "coderequest");


		command.insert("proxytype", "socks5");
		command.insert("proxy_host", proxy_.proxy_host);
		command.insert("proxy_port", proxy_.proxy_port);
		command.insert("username", proxy_.proxy_username);
		command.insert("password", proxy_.proxy_password);
		command.insert("cc", ui->cc->text());
		command.insert("phone", ui->phone->text());

		QJsonDocument document;
		document.setObject(command);
		QByteArray byteArray = document.toJson(QJsonDocument::Compact);
		register_websocket_.sendTextMessage(QString(byteArray));
		register_status_ = kStatusCodeRequest;
	}
}

void RegisterDialog::HandleCoderequestStatus(const QString& message){
	QJsonParseError jsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
	if (!doucment.isObject())
	{
		return;
	}
	QJsonObject root = doucment.object();
	QString status = root["status"].toString();
	if (status != "sent")
	{
		ui->tips->setText(message);
	}
	else{
		ui->tips->setText(QString::fromStdWString(L"输入手机验证码..."));
		ui->code_group->setVisible(true);
		ui->coderequest->setVisible(false);
	}
}

void RegisterDialog::HandleRegisterStatus(const QString& message){
	QJsonParseError jsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(message.toUtf8(), &jsonError);
	if (!doucment.isObject())
	{
		return;
	}
	QJsonObject root = doucment.object();
	QString status = root["status"].toString();
	if (status == "ok")
	{
		ui->tips->setText(QString::fromStdWString(L"注册成功，可以登录了"));
	}
	else
	{
		ui->tips->setText(message);
	}
}
