#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBoxLayout>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), file(new QFile(this)),
      isModified(false), hb(QSharedPointer<HelpBrowser> (new HelpBrowser (":/helpfiles", "index.htm"))) {
  ui->setupUi(this);

  // Заполнение главного меню
  createActions();
  createMenus();

  // Добавление поля для размещения редактируемого текста
  QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::TopToBottom);
  textEdit = new QTextEdit(this);
  boxLayout->addWidget(textEdit, 0);
  ui->centralwidget->setLayout(boxLayout);

  /*! GubaydullinRG
   Привязка события изменения содержимого textEdit к вызову
   слота onTextModified() */
  connect(textEdit, SIGNAL(textChanged()), this, SLOT(onTextModified()));
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::createAction(QAction **action, const QString &actionTitle,
                              const QString &statusTitle,
                              void (MainWindow::*funcSlot)()) {
  *action = new QAction(actionTitle, this);

  (*action)->setStatusTip(statusTitle);

  connect(*action, &QAction::triggered, this, funcSlot);
}

void MainWindow::createActions() {
  // 'File'
  createAction(&newAction, tr("New"), tr("Create new file"),
               &MainWindow::onNew);
  createAction(&openAction, tr("Open"), tr("Open a file"), &MainWindow::onOpen);
  createAction(&closeAction, tr("Close"), tr("Close a file"),
               &MainWindow::onClose);
  createAction(&saveAction, tr("Save"), tr("Save a file"), &MainWindow::onSave);
  createAction(&saveAsAction, tr("Save as"), tr("Save file as"),
               &MainWindow::onSaveAs);
  createAction(&printAction, tr("Print"), tr("Print a file"),
               &MainWindow::onPrint);
  createAction(&exitAction, tr("Exit"), tr("Exit application"),
               &MainWindow::onExit);

  // 'Edit'
  createAction(&copyTextFormatAction, tr("Copy text format"),
               tr("Copy text format"), &MainWindow::onCopyTextFormat);
  createAction(&applyTextFormatAction, tr("Apply text format"),
               tr("Apply text format"), &MainWindow::onApplyTextFormat);
  createAction(&alignTextRightAction, tr("Align text right"),
               tr("Align text right"), &MainWindow::onAlignTextRight);
  createAction(&alignTextLeftAction, tr("Align text left"),
               tr("Align text left"), &MainWindow::onAlignTextLeft);
  createAction(&alignTextCenterAction, tr("Align text center"),
               tr("Align text center"), &MainWindow::onAlignTextCenter);
  createAction(&switchFontAction, tr("Switch font"), tr("Switch font to other"),
               &MainWindow::onSwitchFont);

  // 'Settings'
  createAction(&changeLangAction, tr("Language"),
               tr("Change application language"), &MainWindow::onChangeLang);
  createAction(&changeKeyBindAction, tr("Key bindings"),
               tr("Edit key bindings settings"), &MainWindow::onChangeKeyBind);
  createAction(&changeStyleAction, tr("Change style"),
               tr("Change application style"), &MainWindow::onChangeStyle);

  // '?'
  createAction(&helpAction, tr("Help"), tr("Show application manual"),
               &MainWindow::onHelp);
  createAction(&aboutAction, tr("About"), tr("Short info about application"),
               &MainWindow::onAbout);
}

void MainWindow::createMenus() {
  // 'File'
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAction);
  fileMenu->addAction(openAction);
  fileMenu->addAction(closeAction);
  closeAction->setEnabled(false); // На старте нам нечего закрывать
  fileMenu->addSeparator();
  fileMenu->addAction(saveAction);
  saveAction->setEnabled(false); // На старте нам некуда сохранять
  fileMenu->addAction(saveAsAction);
  fileMenu->addSeparator();
  fileMenu->addAction(printAction);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);

  // 'Edit'
  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(copyTextFormatAction);
  editMenu->addAction(applyTextFormatAction);
  editMenu->addSeparator();
  editMenu->addAction(alignTextRightAction);
  editMenu->addAction(alignTextLeftAction);
  editMenu->addAction(alignTextCenterAction);
  editMenu->addSeparator();
  editMenu->addAction(switchFontAction);

  // 'Settings'
  settingsMenu = menuBar()->addMenu((tr("&Settings")));
  settingsMenu->addAction(changeLangAction);
  settingsMenu->addSeparator();
  settingsMenu->addAction(changeKeyBindAction);
  settingsMenu->addSeparator();
  settingsMenu->addAction(changeStyleAction);

  // '?'
  questionMenu = menuBar()->addMenu("?");
  questionMenu->addAction(helpAction);
  questionMenu->addSeparator();
  questionMenu->addAction(aboutAction);
}

void MainWindow::onSave() {
  if (file->isOpen()) {
    // Проверим режим открытого файла на возможность записи,
    // если нет, то дадим эту возможность
    if (!(file->openMode() & QFile::WriteOnly)) {
      file->close();
      if (!file->open(QIODevice::ReadWrite | QIODevice::Text)) {
        ui->statusbar->showMessage(tr("Can't save file."));
        return;
      }
    }

    QTextStream stream(file);
    stream.seek(0);
    stream << textEdit->toPlainText();

    ui->statusbar->showMessage(file->fileName() + " " + tr("has been saved."));

    isModified = false;
  } else
  // На случай, если никакой файл в textEdit не загружен,
  // но юзер хочет сохранить содержимое textEdit в файл,
  {
    onSaveAs();
  }

  saveAction->setEnabled(false);
}

void MainWindow::onSaveAs() {
  QString filePath{QFileDialog::getSaveFileName(this, tr("Save file as "),
                                                QDir::current().path(),
                                                tr("Text file(*.txt)"))};

  if (filePath.length()) {
    if (file->isOpen())
      file->close();

    file->setFileName(filePath);
    if (file->open(QFile::WriteOnly)) {
      QTextStream stream(file);

      stream << textEdit->toPlainText();

      ui->statusbar->showMessage(tr("File saved as ") + file->fileName() + '.');

      isModified = false;
    } else //! open
    {
      QMessageBox::warning(this, tr("Can't save file"),
                           tr("Cannot save file ") + filePath);
    }
  }
}

void MainWindow::onPrint() {}

void MainWindow::onExit() 
{ 
    if (!isTextModified)
        MainWindow::close();
    else if (warningWindow())
        MainWindow::close(); 
}

void MainWindow::onCopyTextFormat() {}

void MainWindow::onApplyTextFormat() {}

void MainWindow::onAlignTextRight() {}

void MainWindow::onAlignTextLeft() {}

void MainWindow::onAlignTextCenter() {}

void MainWindow::onSwitchFont() {}

void MainWindow::onChangeLang() {}

void MainWindow::onChangeKeyBind() {}

void MainWindow::onChangeStyle() {
  QString newStyle = "white";
  if (currentStyle == newStyle) {
    newStyle = "grey";
  }
  QFile qss(":/themes/" + newStyle + ".qss");
  if (!qss.open(QIODevice::ReadOnly))
    return;
  qApp->setStyleSheet(qss.readAll());
  qss.close();
  currentStyle = newStyle;
}

void MainWindow::onNew()
{
    textEdit->clear();
    textEdit->setHidden(false);
    lastFilename = "file.txt";
}

void MainWindow::onOpen()
{

    QString fileName;
    fileName = QFileDialog::getOpenFileName(this, tr("Open Document"),
    QDir::currentPath(), "All files (*.*) ;; Document files (*.txt)");
    if(fileName == "file.txt")
        {
            return;
        }
        else
        {
            QFile file(fileName);
            if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
            {
             QMessageBox::warning(this,tr("Error"),tr("Open failed"));
    return;
    }
    else
    {
    if(!file.isReadable())
    {
    QMessageBox::warning(this,tr("Error"),tr("The file is not read"));
    }
    else
    {
    QTextStream textStream(&file);
    while(!textStream.atEnd())
    {
    textEdit->setPlainText(textStream.readAll());
    }
    textEdit->show();
    file.close();
    lastFilename = fileName;
                   }
               }
            }

}

void MainWindow::onClose()
{
    if (isTextModified)
    {
        if (warningWindow())
        {
            textEdit->clear();
            isTextModified = false;
            closeAction->setEnabled(false);
        }
    }
}

void MainWindow::onHelp()
{
    hb->resize(600,400);
    hb->show();
}

void MainWindow::onAbout()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("О программе");
    msgBox.setIconPixmap(QPixmap(":/images/about.png"));

    msgBox.setInformativeText(" ПО Текстовый редактор v 0.0 \n\n"

                              "  GB_CommandProgCPP_team3\n\n"

                              "© 2008-2022 The Qt Company Ltd.\n "
                              "     Все права защищены.\n\n");
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
}

/*! GubaydullinRG
        Выполнение действий в случае изменения
        содержимого textEdit */
void MainWindow::onTextModified() {
  isModified = true;
  saveAction->setEnabled(true);
}

bool MainWindow::warningWindow()
{
    QMessageBox choice; // Создаём диалоговое окно
    choice.setWindowTitle(tr("Вы уверены?"));
    choice.setText(tr("Все несохраненные данные будут утеряны!"));
    choice.addButton(tr("Да"), QMessageBox::YesRole);
    choice.addButton(tr("Нет"), QMessageBox::NoRole);
    if (choice.exec() == false){
         return true;
    } else {
        choice.close();
        return false;
    }
}

void MainWindow::changeEnableActions() // Переключение активности режима кнопки
{
    isTextModified = true;
    closeAction->setEnabled(true);
}
