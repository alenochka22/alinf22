#include <QApplication>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>

class DeviceManager : public QWidget
{
    Q_OBJECT

public:
    DeviceManager(QWidget *parent = nullptr);

private:
    void loadDeviceInfo();
    void loadDriverInfo(const QString &deviceId);
    void runCommand(const QString &command, QStringList arguments);

    QListWidget *deviceList;
    QLabel *deviceInfoLabel;
    QLabel *driverInfoLabel;
};

DeviceManager::DeviceManager(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    deviceList = new QListWidget(this);
    layout->addWidget(deviceList);

    deviceInfoLabel = new QLabel("Device Info", this);
    layout->addWidget(deviceInfoLabel);

    driverInfoLabel = new QLabel("Driver Info", this);
    layout->addWidget(driverInfoLabel);

    loadDeviceInfo();

    connect(deviceList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        loadDriverInfo(item->text());
    });
}

void DeviceManager::loadDeviceInfo()
{
    // Здесь вызываем утилиту lspci или inxi для получения списка устройств
    QString command = "lspci";
    runCommand(command, QStringList());
}

void DeviceManager::loadDriverInfo(const QString &deviceId)
{
    // Здесь выводим информацию о драйвере устройства и возможных альтернативных драйверах
    QString command = "lspci -v -s " + deviceId;
    runCommand(command, QStringList());
}

void DeviceManager::runCommand(const QString &command, QStringList arguments)
{
    QProcess *process = new QProcess(this);
    process->setProgram(command);
    process->setArguments(arguments);
    process->start();
    process->waitForFinished();

    QString output = process->readAllStandardOutput();
    deviceInfoLabel->setText(output);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceManager manager;
    manager.show();

    return app.exec();
}

#include "main.moc"
