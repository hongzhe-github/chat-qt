// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 创建一个 QShortcut 对象，连接回车键和按钮点击事件
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Return), this);

    // 连接回车键和执行按钮点击事件的操作
    connect(shortcut, &QShortcut::activated, ui->pushButton, &QPushButton::click);

    connect(&m_webSocket, &QWebSocket::connected, this, &MainWindow::onWebSocketConnected);
    connect(&m_webSocket, static_cast<void(QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error), this, &MainWindow::onWebSocketError);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &MainWindow::onWebSocketTextMessageReceived);
    m_webSocket.open(QUrl("ws://hongzhe.xyz:8000/ws/chat/lobby/"));

    // 读取聊天记录
    QByteArray arrayData = sendHttpRequest("http://hongzhe.xyz:8000/chat/lobby/messages/");
    QJsonArray chatRecords = parseJsonResponse(arrayData);

    // 显示聊天记录
    for (const QJsonValue& recordValue : chatRecords) {
        if (recordValue.isObject()) {
            QJsonObject record = recordValue.toObject();
            QString username = record["username"].toString();
            QString msg = record["content"].toString();
            ui->textBrowser->append(username + ": " + msg);
        }
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendMessage(QString message)
{
    // 创建JSON对象
    QJsonObject json;
    json["message"] = message;
    json["username"] = "游客(qt客户端)";
    json["room"] = "lobby";

    // 将JSON对象转换为JSON文档
    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    // 发送JSON字符串到WebSocket服务器
    m_webSocket.sendTextMessage(jsonString);
    qDebug() << "发送消息:" << message;
}

QJsonObject parseJsonFromString(const QString& jsonString)
{
    QJsonObject jsonObject;

    QJsonParseError parseError;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (parseError.error == QJsonParseError::NoError && !jsonDocument.isNull() && jsonDocument.isObject())
    {
        jsonObject = jsonDocument.object();
    }
    else
    {
        qDebug() << "JSON parse error:" << parseError.errorString();
    }

    return jsonObject;
}

// 发送消息按钮
void MainWindow::on_pushButton_clicked()
{
    QString message = ui->plainTextEdit->toPlainText();
    sendMessage(message);
    ui->plainTextEdit->clear();

}

void MainWindow::onWebSocketConnected()
{
    qDebug() << "WebSocket 已连接";
}

void MainWindow::onWebSocketError(QAbstractSocket::SocketError error)
{
    qDebug() << "WebSocket 错误:" << error;
}



void MainWindow::onWebSocketTextMessageReceived(const QString &message)
{

    // 在textBrowser中显示收到的消息
    QJsonObject jsonObject = parseJsonFromString(message);

    if (!jsonObject.isEmpty())
    {
        QString username = jsonObject["username"].toString();
        QString msg = jsonObject["message"].toString();
        QString room = jsonObject["room"].toString();
        ui->textBrowser->append(msg);
        qDebug() << "Received message:" << msg;
    }
}



// 发送HTTP GET请求并返回响应
QByteArray MainWindow::sendHttpRequest(const QString& url) {
    // 创建QNetworkAccessManager对象
    QNetworkAccessManager manager;

    // 发送GET请求
    QNetworkReply* reply = manager.get(QNetworkRequest(QUrl(url)));

    // 等待请求完成
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    // 获取响应数据
    QByteArray response = reply->readAll();

    // 清理资源
    reply->deleteLater();

    return response;
}

// 解析JSON响应为QJsonObject对象
QJsonArray MainWindow::parseJsonResponse(const QByteArray& response) {
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(response, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误: " << error.errorString();
                                            return QJsonArray();
    }

    if (!document.isArray()) {
        qDebug() << "JSON响应不是数组格式";
        qDebug() << "响应内容：" << response;
            return QJsonArray();
    }

    return document.array();
}

void MainWindow::on_pushButtonClose_clicked()
{
    this->close();
}

