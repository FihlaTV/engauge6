#ifndef DOCUMENT_MODEL_GRID_DISPLAY_H
#define DOCUMENT_MODEL_GRID_DISPLAY_H

#include "DocumentModelAbstractBase.h"
#include "GridCoordDisable.h"

class CmdMediator;

/// Model for DlgSettingsGridDisplay and CmdSettingsGridDisplay.
class DocumentModelGridDisplay : public DocumentModelAbstractBase
{
public:
  /// Default constructor.
  DocumentModelGridDisplay();

  /// Initial constructor from Document.
  DocumentModelGridDisplay(const CmdMediator &cmdMediator);

  /// Copy constructor.
  DocumentModelGridDisplay(const DocumentModelGridDisplay &other);

  /// Assignment constructor.
  DocumentModelGridDisplay &operator=(const DocumentModelGridDisplay &other);

  /// Get method for x count.
  int countX() const;

  /// Get method for y count.
  int countY() const;

  /// Get method for x coord parameter to disable.
  GridCoordDisable gridCoordDisableX () const;

  /// Get method for y coord parameter to disable.
  GridCoordDisable gridCoordDisableY () const;

  virtual void saveModel(QXmlStreamWriter &stream) const;

  /// Set method for x count.
  void setCountX(int countX);

  /// Set method for y count.
  void setCountY(int countY);

  /// Get method for x coord parameter to disable.
  void setGridCoordDisableX (GridCoordDisable gridCoordDisable);

  /// Get method for y coord parameter to disable.
  void setGridCoordDisableY (GridCoordDisable gridCoordDisable);

  /// Set method for x start.
  void setStartX(double startX);

  /// Set method for y start.
  void setStartY(double startY);

  /// Set method for x step.
  void setStepX(double stepX);

  /// Set method for y step.
  void setStepY(double stepY);

  /// Set method for x stop.
  void setStopX(double stopX);

  /// Set method for y stop.
  void setStopY(double stopY);

  /// Get method for x start.
  double startX() const;

  /// Get method for y start.
  double startY() const;

  /// Get method for x step.
  double stepX() const;

  /// Get method for y step.
  double stepY() const;

  /// Get method for x stop.
  double stopX() const;

  /// Get method for y stop.
  double stopY() const;

private:

  GridCoordDisable m_gridCoordDisableX;
  int m_countX;
  double m_startX;
  double m_stepX;
  double m_stopX;
  GridCoordDisable m_gridCoordDisableY;
  int m_countY;
  double m_startY;
  double m_stepY;
  double m_stopY;
};

#endif // DOCUMENT_MODEL_GRID_DISPLAY_H