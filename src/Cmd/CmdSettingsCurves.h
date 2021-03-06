#ifndef CMD_SETTINGS_CURVES_H
#define CMD_SETTINGS_CURVES_H

#include "CmdAbstract.h"
#include "CurvesGraphs.h"

class DocumentModelCurves;

/// Command for DlgSettingsCurves
class CmdSettingsCurves : public CmdAbstract
{
public:
  /// Single constructor.
  CmdSettingsCurves(MainWindow &mainWindow,
                    Document &document,
                    const DocumentModelCurves &modelCurves);

  virtual void cmdRedo ();
  virtual void cmdUndo ();

private:
  CmdSettingsCurves();

  CurvesGraphs m_curvesGraphsBefore;
  CurvesGraphs m_curvesGraphsAfter;
};

#endif // CMD_SETTINGS_CURVES_H
