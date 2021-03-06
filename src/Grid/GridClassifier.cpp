#include "Correlation.h"
#include "Filter.h"
#include "GridClassifier.h"
#include "Logger.h"
#include <QDebug>
#include <QPixmap>
#include "QtToString.h"
#include "Transformation.h"

const int MIN_STEP_PIXELS = 5;
const double PEAK_HALF_WIDTH = 4;

GridClassifier::GridClassifier()
{
}

void GridClassifier::classify (const QPixmap &originalPixmap,
                               const Transformation &transformation,
                               int &countX,
                               double &startX,
                               double &stepX,
                               int &countY,
                               double &startY,
                               double &stepY)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GridClassifier::classify";

  QImage image = originalPixmap.toImage ();

  double xMin, xMax, yMin, yMax;
  double binStartX, binStepX, binStartY, binStepY;

  computeGraphCoordinateLimits (image,
                                transformation,
                                xMin,
                                xMax,
                                yMin,
                                yMax);
  initializeHistogramBins ();
  populateHistogramBins (image,
                         transformation,
                         xMin,
                         xMax,
                         yMin,
                         yMax);
  searchStartStepSpace (xMin,
                        xMax,
                        yMin,
                        yMax,
                        startX,
                        stepX,
                        startY,
                        stepY,
                        binStartX,
                        binStepX,
                        binStartY,
                        binStepY);
  searchCountSpace (m_binsX,
                    binStartX,
                    binStepX,
                    countX);
  searchCountSpace (m_binsY,
                    binStartY,
                    binStepY,
                    countY);
}

void GridClassifier::computeGraphCoordinateLimits (const QImage &image,
                                                   const Transformation &transformation,
                                                   double &xMin,
                                                   double &xMax,
                                                   double &yMin,
                                                   double &yMax)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GridClassifier::computeGraphCoordinateLimits";

  // Project every pixel onto the x axis, and onto the y axis. One set of histogram bins will be
  // set up along each of the axes. The range of bins will encompass every pixel in the image, and no more.

  QPointF posGraphTL, posGraphTR, posGraphBL, posGraphBR;
  transformation.transform (QPointF (0, 0)                         , posGraphTL);
  transformation.transform (QPointF (image.width(), 0)             , posGraphTR);
  transformation.transform (QPointF (0, image.height())            , posGraphBL);
  transformation.transform (QPointF (image.width(), image.height()), posGraphBR);

  // Since transformation is affine (linear rather than quadratic), we only need to look at the screen corners to
  // get the domains of x and y
  xMin = qMin (qMin (qMin (posGraphTL.x(), posGraphTR.x()), posGraphBL.x()), posGraphBR.x());
  xMax = qMax (qMax (qMax (posGraphTL.x(), posGraphTR.x()), posGraphBL.x()), posGraphBR.x());
  yMin = qMin (qMin (qMin (posGraphTL.y(), posGraphTR.y()), posGraphBL.y()), posGraphBR.y());
  yMax = qMax (qMax (qMax (posGraphTL.y(), posGraphTR.y()), posGraphBL.y()), posGraphBR.y());

  Q_ASSERT (xMin < xMax);
  Q_ASSERT (yMin < yMax);
}

void GridClassifier::initializeHistogramBins ()
{
  LOG4CPP_INFO_S ((*mainCat)) << "GridClassifier::initializeHistogramBins";

  for (int bin = 0; bin < NUM_HISTOGRAM_BINS; bin++) {
    m_binsX [bin] = 0;
    m_binsY [bin] = 0;
  }
}

void GridClassifier::loadPicketFence (double picketFence [NUM_HISTOGRAM_BINS],
                                      int binStart,
                                      int binStep,
                                      int count,
                                      bool isCount)
{
  // Count argument is optional
  int binStop = 0;
  if (isCount) {
    binStop = binStart + count * binStep;
  }

  for (int bin = 0; bin < NUM_HISTOGRAM_BINS; bin++) {

    // If nothing is happening, bin is zero
    picketFence [bin] = 0;

    if ((bin < binStop) || !isCount) {

      // Set up picket fence of evenly spaced peaks. First peak is always at bin=binStart. Originally
      // each peak was one bin wide (=an impulse function) but that was unstable so then
      // each peak was modeled as a triangle
      int modValue = (bin - binStart) % binStep;
      if (modValue < PEAK_HALF_WIDTH) {

        // Map 0 to PEAK_HALF_WIDTH to 1 to 0
        picketFence [bin] = 1.0 - (double) modValue / PEAK_HALF_WIDTH;

      } else if (binStep - modValue < PEAK_HALF_WIDTH) {

        // Map binStep-0 to binStep-PEAK_HALF_WIDTH to 1 to 0
        picketFence [bin] = 1.0 - (double) (binStep - modValue) / PEAK_HALF_WIDTH;

      }
    }
  }
}

void GridClassifier::populateHistogramBins (const QImage &image,
                                            const Transformation &transformation,
                                            double xMin,
                                            double xMax,
                                            double yMin,
                                            double yMax)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GridClassifier::populateHistogramBins";

  Filter filter;
  QRgb rgbBackground = filter.marginColor (&image);

  for (int x = 0; x < image.width(); x++) {
    for (int y = 0; y < image.height(); y++) {

      QColor pixel = image.pixel (x, y);

      // Skip pixels with background color
      if (!filter.colorCompare (rgbBackground,
                                pixel.rgb ())) {

        // Add this pixel to histograms
        QPointF posGraph;
        transformation.transform (QPointF (x, y), posGraph);
        int binX = (NUM_HISTOGRAM_BINS - 1.0) * (posGraph.x() - xMin) / (xMax - xMin);
        int binY = (NUM_HISTOGRAM_BINS - 1.0) * (posGraph.y() - yMin) / (yMax - yMin);

        Q_ASSERT (binX < NUM_HISTOGRAM_BINS);
        Q_ASSERT (binY < NUM_HISTOGRAM_BINS);

        ++m_binsX [binX];
        ++m_binsY [binY];
      }
    }
  }
}

void GridClassifier::searchCountSpace (double bins [NUM_HISTOGRAM_BINS],
                                       double binStart,
                                       double binStep,
                                       int &countMax)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GridClassifier::searchCountSpace";

  // Loop though the space of possible counts
  Correlation correlation (NUM_HISTOGRAM_BINS);
  double picketFence [NUM_HISTOGRAM_BINS];
  double corr, corrMax;
  bool isFirst = true;
  int countStop = 1 + (NUM_HISTOGRAM_BINS - binStart) / binStep;
  for (int count = 2; count <= countStop; count++) {

    loadPicketFence (picketFence,
                     binStart,
                     binStep,
                     count,
                     true);

    correlation.correlateWithoutShift (NUM_HISTOGRAM_BINS,
                                       bins,
                                       picketFence,
                                       corr);
    if (isFirst || (corr > corrMax)) {
      countMax = count;
      corrMax = corr;
    }

    isFirst = false;
  }
}

void GridClassifier::searchStartStepSpace (double xMin,
                                           double xMax,
                                           double yMin,
                                           double yMax,
                                           double &startX,
                                           double &stepX,
                                           double &startY,
                                           double &stepY,
                                           double &binStartXMax,
                                           double &binStepXMax,
                                           double &binStartYMax,
                                           double &binStepYMax)
{
  LOG4CPP_INFO_S ((*mainCat)) << "GridClassifier::searchStartStepSpace";

  // Loop though the space of possible gridlines using the independent variables (start,step).
  Correlation correlation (NUM_HISTOGRAM_BINS);
  double picketFence [NUM_HISTOGRAM_BINS];
  int binStartX, binStartY;
  double corrX, corrY, corrXMax, corrYMax;
  bool isFirst = true;
  for (int binStep = MIN_STEP_PIXELS; binStep < NUM_HISTOGRAM_BINS; binStep++) {

    const int BIN_START = 0;
    loadPicketFence (picketFence,
                     BIN_START,
                     binStep,
                     0,
                     false);

    correlation.correlateWithShift (NUM_HISTOGRAM_BINS,
                                    m_binsX,
                                    picketFence,
                                    binStartX,
                                    corrX);
    correlation.correlateWithShift (NUM_HISTOGRAM_BINS,
                                    m_binsY,
                                    picketFence,
                                    binStartY,
                                    corrY);
    if (isFirst || (corrX > corrXMax)) {
      binStartXMax = binStartX;
      binStepXMax = binStep;
      corrXMax = corrX;
    }

    if (isFirst || (corrY > corrYMax)) {
      binStartYMax = binStartY;
      binStepYMax = binStep;
      corrYMax = corrY;
    }
    isFirst = false;
  }

  // Convert from bins back to graph coordinates
  startX = xMin + binStartXMax / (NUM_HISTOGRAM_BINS - 1.0) * (xMax - xMin);
  startY = yMin + binStartYMax / (NUM_HISTOGRAM_BINS - 1.0) * (yMax - yMin);
  stepX = binStepXMax / (NUM_HISTOGRAM_BINS - 1.0) * (xMax - xMin);
  stepY = binStepYMax / (NUM_HISTOGRAM_BINS - 1.0) * (yMax - yMin);
}
