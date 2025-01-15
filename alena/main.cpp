#include <QApplication>
#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QRegularExpression>

class DeviceManager : public QWidget
{
    Q_OBJECT

public:
    DeviceManager(QWidget *parent = nullptr);

private:
    void loadDeviceList();
    void loadDriverInfo(const QString &deviceId);

    QListWidget *deviceList;
    QLabel *driverInfoLabel;
};

DeviceManager::DeviceManager(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Список устройств
    deviceList = new QListWidget(this);
    layout->addWidget(deviceList);

    // Информация о драйверах
    driverInfoLabel = new QLabel("Выберите устройство для отображения информации.", this);
    driverInfoLabel->setStyleSheet("background-color: white; color: black; font-family: 'Courier New'; font-size: 10pt;");
    driverInfoLabel->setWordWrap(true); // Включаем перенос строк
    layout->addWidget(driverInfoLabel);

    // Загружаем список устройств
    loadDeviceList();

    // Подключаем обработчик кликов по элементу списка
    connect(deviceList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        loadDriverInfo(item->text().split(" ")[0]); // Загружаем информацию о выбранном устройстве
    });

    // Если список не пуст, загружаем информацию о текущем выделенном устройстве
    if (deviceList->count() > 0) {
        deviceList->setCurrentRow(0); // Устанавливаем выделение на первый элемент
        QListWidgetItem *firstItem = deviceList->item(0);
        loadDriverInfo(firstItem->text().split(" ")[0]); // Загружаем информацию о нём
    }
}

void DeviceManager::loadDeviceList()
{
    QProcess process;
    process.start("lspci");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    QStringList devices = output.split("\n", Qt::SkipEmptyParts);
    for (const QString &device : devices) {
        deviceList->addItem(device); // Добавляем устройство в список
    }
}

void DeviceManager::loadDriverInfo(const QString &deviceId)
{
    QProcess process;
    process.start("lspci", QStringList() << "-n" << "-s" << deviceId); // Получаем PCI ID устройства
    process.waitForFinished();
    QString pciIdOutput = process.readAllStandardOutput();
    QString pciId = pciIdOutput.split(" ")[2].trimmed(); // Извлекаем PCI ID

    QString deviceName;
    QString subsystem;
    QString kernelDriver;
    QString kernelModules;
    QString alternativeDrivers;

    // Анализ текущего драйвера и связанных модулей
    process.start("lspci", QStringList() << "-v" << "-s" << deviceId);
    process.waitForFinished();
    QString output = process.readAllStandardOutput();
    for (const QString &line : output.split("\n")) {
        if (line.contains("Kernel driver in use:")) {
            kernelDriver = line.section(":", 1).trimmed();
        } else if (line.contains("Kernel modules:")) {
            kernelModules = line.section(":", 1).trimmed();
        } else if (!line.startsWith("\t") && !line.isEmpty()) {
            deviceName = line.trimmed(); // Первая строка — название устройства
        }
    }

    // Анализ модулей и поиск совместимых драйверов
    if (!kernelModules.isEmpty()) {
        QStringList modules = kernelModules.split(",", Qt::SkipEmptyParts);
        modules.replaceInStrings(QRegularExpression("^\\s+|\\s+$"), ""); // Убираем пробелы

        for (const QString &module : modules) {
            QProcess modinfoProcess;
            modinfoProcess.start("modinfo", QStringList() << module);
            modinfoProcess.waitForFinished();
            QString modinfoOutput = modinfoProcess.readAllStandardOutput();

            if (modinfoOutput.contains(pciId)) { // Проверяем, поддерживает ли драйвер данное устройство
                if (module != kernelDriver) { // Исключаем текущий используемый драйвер
                    alternativeDrivers += module + ", ";
                }
            }
        }

        if (!alternativeDrivers.isEmpty()) {
            alternativeDrivers.chop(2); // Убираем последнюю запятую и пробел
        }
    }

    // Форматируем вывод
    QString info = deviceName + "\n";
    info += "Kernel driver in use: " + (kernelDriver.isEmpty() ? "N/A" : kernelDriver) + "\n";
    info += "Kernel modules: " + (kernelModules.isEmpty() ? "N/A" : kernelModules) + "\n";
    info += "Alternative drivers: " + (alternativeDrivers.isEmpty() ? "None" : alternativeDrivers) + "\n";

    // Выводим информацию
    driverInfoLabel->setText(info);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DeviceManager manager;
    manager.show();

    return app.exec();
}

#include "main.moc"
