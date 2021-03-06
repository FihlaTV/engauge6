#include "LoadImageFromUrl.h"
#include "Logger.h"
#include "MainWindow.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QtNetwork/QNetworkReply>
#include <QUrl>

LoadImageFromUrl::LoadImageFromUrl (MainWindow &mainWindow) :
  m_mainWindow (mainWindow),
  m_http (this),
  m_reply (0),
  m_buffer (0)
{
  connect (this, SIGNAL (signalImportImage (QString, QImage)), &m_mainWindow, SLOT (slotFileImportImage (QString, QImage)));
}

LoadImageFromUrl::~LoadImageFromUrl ()
{
  deallocate ();
}

void LoadImageFromUrl::deallocate ()
{
  if (m_reply != 0) {
    delete m_reply;
    m_reply = 0;
  }

  if (m_buffer != 0) {
    delete m_buffer;
    m_buffer = 0;
  }
}

void LoadImageFromUrl::slotFinished ()
{
  // Download has just finished

  QString urlWithoutScheme = m_url.toString (QUrl::RemoveScheme);

  // Import
  QImage image;
  if (image.loadFromData (*m_buffer)) {

    emit signalImportImage (urlWithoutScheme,
                            image);
  } else {

    // Images embedded in web pages produce html in m_buffer. No easy way to fix that. Even
    // gimp fails in the same situations so we just show an error

    QString message;
    QTextStream str (&message);

    str << "Unable to download image from " << urlWithoutScheme;

    QMessageBox::critical (&m_mainWindow,
                           tr ("Engauge Digitizer"),
                           message,
                           QMessageBox::Ok);
  }
}

void LoadImageFromUrl::startLoadImage (const QUrl &url)
{
  LOG4CPP_INFO_S ((*mainCat)) << "LoadImageFromUrl::startLoadImage url=" << url.toString ().toLatin1 ().data ();

  m_url = url;
  if (url.isLocalFile ()) {

    QFileInfo fileInfo (url.toLocalFile ());

    // Load local file. This is done synchronously
    QImage image;
    if (image.load (url.toLocalFile ())) {

      emit signalImportImage (fileInfo.fileName (),
                              image);

    } else {

      // Probably a bad file type

      QString message;
      QTextStream str (&message);

      str << "Unable to load image from " << url.toLocalFile ();

      QMessageBox::critical (&m_mainWindow,
                             tr ("Engauge Digitizer"),
                             message,
                             QMessageBox::Ok);
    }

  } else {

    // Asynchronous read from url
    deallocate ();
    m_buffer = new QByteArray;
    QNetworkRequest request (url);
    m_reply = m_http.get (request);

    connect (m_reply, SIGNAL (readyRead()), this, SLOT (slotReadData()));
    connect (m_reply, SIGNAL (finished ()), this, SLOT (slotFinished ()));
  }
}

void LoadImageFromUrl::slotReadData ()
{
  *m_buffer += m_reply->readAll ();
}
