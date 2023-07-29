
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWebSockets/QWebSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QShortcut>


#include <QtNetwork>
#include <QJsonParseError>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static QByteArray sendHttpRequest(const QString& url);
    static QJsonArray parseJsonResponse(const QByteArray& response);

private slots:
    void on_pushButton_clicked();
    void onWebSocketConnected();
    void onWebSocketError(QAbstractSocket::SocketError error);
    void onWebSocketTextMessageReceived(const QString &message);


    void on_pushButtonClose_clicked();

private:
    Ui::MainWindow *ui;
    QWebSocket m_webSocket;

private:
    void sendMessage(QString message);
};

#endif // MAINWINDOW_H
