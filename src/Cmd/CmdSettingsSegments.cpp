#include "CmdSettingsSegments.h"
#include "Document.h"
#include "Logger.h"
#include "MainWindow.h"

CmdSettingsSegments::CmdSettingsSegments(MainWindow &mainWindow,
                                         Document &document,
                                         const DocumentModelSegments &modelSegmentsBefore,
                                         const DocumentModelSegments &modelSegmentsAfter) :
  CmdAbstract(mainWindow,
              document,
              "Segments settings"),
  m_modelSegmentsBefore (modelSegmentsBefore),
  m_modelSegmentsAfter (modelSegmentsAfter)
{
  LOG4CPP_INFO_S ((*mainCat)) << "CmdSettingsSegments::CmdSettingsSegments";
}

void CmdSettingsSegments::cmdRedo ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "CmdSettingsSegments::cmdRedo";

  mainWindow().updateSettingsSegments(m_modelSegmentsAfter);
  mainWindow().updateAfterCommand();
}

void CmdSettingsSegments::cmdUndo ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "CmdSettingsSegments::cmdUndo";

  mainWindow().updateSettingsSegments(m_modelSegmentsBefore);
  mainWindow().updateAfterCommand();
}
