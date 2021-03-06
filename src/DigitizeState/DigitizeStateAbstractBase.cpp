#include "CmdEditPointAxis.h"
#include "CmdMediator.h"
#include "DigitizeStateAbstractBase.h"
#include "DigitizeStateContext.h"
#include "DlgEditPoint.h"
#include "Logger.h"
#include "MainWindow.h"
#include <QApplication>
#include <QGraphicsScene>
#include <QImage>
#include <QMessageBox>
#include <QTimer>
#include "QtToString.h"

DigitizeStateAbstractBase::DigitizeStateAbstractBase(DigitizeStateContext &context) :
  m_context (context),
  m_isOverrideCursor (false)
{
}

DigitizeStateAbstractBase::~DigitizeStateAbstractBase()
{
}

DigitizeStateContext &DigitizeStateAbstractBase::context()
{
  return m_context;
}

const DigitizeStateContext &DigitizeStateAbstractBase::context() const
{
  return m_context;
}

void DigitizeStateAbstractBase::handleContextMenuEvent (const QString &pointIdentifier)
{
  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStateAbstractBase::handleContextMenuEvent point=" << pointIdentifier.toLatin1 ().data ();

  QPointF posScreen = context().cmdMediator().document().positionScreen (pointIdentifier);
  QPointF posGraphBefore = context().cmdMediator().document().positionGraph (pointIdentifier);
  QString xGraphValue = QString ("%1").arg (posGraphBefore.x ());
  QString yGraphValue = QString ("%1").arg (posGraphBefore.y ());

  // Ask user for coordinates
  DlgEditPoint *dlg = new DlgEditPoint(context().mainWindow(),
                                       *this,
                                       cursorShape (),
                                       xGraphValue,
                                       yGraphValue);
  int rtn = dlg->exec ();

  QPointF posGraphAfter = dlg->posGraph ();
  delete dlg;

  if (rtn == QDialog::Accepted) {

    // User wants to edit this axis point, but let's perform sanity checks first

    bool isError;
    QString errorMessage;

    context().mainWindow().cmdMediator().document().checkEditPointAxis(pointIdentifier,
                                                                       posScreen,
                                                                       posGraphAfter,
                                                                       isError,
                                                                       errorMessage);

    if (isError) {

      QMessageBox::warning (0,
                            "Application",
                            errorMessage);

    } else {


      CmdEditPointAxis *cmd = new CmdEditPointAxis (context().mainWindow(),
                                                    context().cmdMediator().document(),
                                                    pointIdentifier,
                                                    posGraphBefore,
                                                    posGraphAfter);
      context().appendNewCmd(cmd);
    }
  }
}

void DigitizeStateAbstractBase::handleLeave ()
{
  LOG4CPP_DEBUG_S ((*mainCat)) << "DigitizeStateAbstractBase::handleLeave";

  removeOverrideCursor ();
}

void DigitizeStateAbstractBase::handleSetOverrideCursor (Qt::CursorShape cursorShape)
{
  removeOverrideCursor ();

  LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStateAbstractBase::handleSetOverrideCursor setOverrideCursor="
                              << QtCursorToString (cursorShape).toLatin1 ().data ();

  QApplication::setOverrideCursor (cursorShape);
  m_isOverrideCursor = true;
}

void DigitizeStateAbstractBase::removeOverrideCursor ()
{
  if (m_isOverrideCursor) {

    LOG4CPP_INFO_S ((*mainCat)) << "DigitizeStateAbstractBase::handleLeave restoreOverrideCursor="
                                << QtCursorToString (QApplication::overrideCursor ()->shape ()).toLatin1 ().data ();

    // Override cursor from last QDialog must be restored
    QApplication::restoreOverrideCursor ();

    m_isOverrideCursor = false;
  }
}

void DigitizeStateAbstractBase::setCursor()
{
  LOG4CPP_DEBUG_S ((*mainCat)) << "DigitizeStateAbstractBase::setCursor";

  removeOverrideCursor ();
  context().view().setCursor (QCursor (cursorShape ()));
}
