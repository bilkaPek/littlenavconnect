/*****************************************************************************
* Copyright 2015-2016 Alexander Barthel albar965@mailbox.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>

namespace Ui {
class MainWindow;
}

namespace atools {
namespace gui {
class HelpHandler;
}
}

class NavServer;
class DataReaderThread;

class MainWindow :
  public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  virtual ~MainWindow();

signals:
  void appendLogMessage(const QString& message);

  /* Emitted when window is shown the first time */
  void windowShown();

private:
  void logGuiMessage(QtMsgType type, const QMessageLogContext& context, const QString& message);
  virtual void showEvent(QShowEvent *event) override;

  Ui::MainWindow *ui;
  NavServer *navServer;
  atools::gui::HelpHandler *helpHandler;
  DataReaderThread *dataReader = nullptr;
  bool firstStart = true;
  virtual void closeEvent(QCloseEvent *event) override;

  void readSettings();
  void writeSettings();
  void mainWindowShown();
  void resetMessages();
  void options();

};

#endif // MAINWINDOW_H
