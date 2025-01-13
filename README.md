#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QDebug>  // Для использования qDebug()

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent),
        listWidget(new QListWidget(this)),
        pushButton(new QPushButton("Update Device List", this)),
        label(new QLabel("Selected Device: None", this))
    {
        // Устанавливаем центральный виджет и его компоновку
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // Создаем компоновку
        QVBoxLayout *layout = new QVBoxLayout(centralWidget);

        // Добавляем виджеты и добавляем отладочный вывод
        qDebug() << "Adding listWidget to layout...";
        layout->addWidget(listWidget);

        qDebug() << "Adding pushButton to layout...";
        layout->addWidget(pushButton);

        qDebug() << "Adding label to layout...";
        layout->addWidget(label);

        // Связь сигналов и слотов
        connect(pushButton, &QPushButton::clicked, this, &MainWindow::updateDeviceList);
        connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::displayDeviceInfo);

        // Устанавливаем размер окна
        setWindowTitle("Device Manager");
        resize(400, 300);
    }

private slots:
    void updateDeviceList()
    {
        // Очистить список
        listWidget->clear();

        // Пример добавления устройств в список (можно заменить на реальную логику получения устройств)
        listWidget->addItem("Device 1: USB Device");
        listWidget->addItem("Device 2: Video Card");
        listWidget->addItem("Device 3: Network Adapter");
    }

    void displayDeviceInfo(QListWidgetItem *item)
    {
        QString deviceName = item->text();
        label->setText("Selected Device: " + deviceName);
    }

private:
    QListWidget *listWidget;   // Список устройств
    QPushButton *pushButton;   // Кнопка обновления
    QLabel *label;             // Метка для отображения выбранного устройства
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
