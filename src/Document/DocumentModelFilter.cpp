#include "CmdMediator.h"
#include "DocumentModelFilter.h"
#include "Logger.h"
#include <QXmlStreamWriter>

DocumentModelFilter::DocumentModelFilter() :
  m_filterParameter (FILTER_PARAMETER_INTENSITY),
  m_intensityLow (INTENSITY_LOW_DEFAULT),
  m_intensityHigh (INTENSITY_HIGH_DEFAULT),
  m_foregroundLow (FOREGROUND_LOW_DEFAULT),
  m_foregroundHigh (FOREGROUND_HIGH_DEFAULT),
  m_hueLow (HUE_LOW_DEFAULT),
  m_hueHigh (HUE_HIGH_DEFAULT),
  m_saturationLow (SATURATION_LOW_DEFAULT),
  m_saturationHigh (SATURATION_HIGH_DEFAULT),
  m_valueLow (VALUE_LOW_DEFAULT),
  m_valueHigh (VALUE_HIGH_DEFAULT)
{
}

DocumentModelFilter::DocumentModelFilter(const Document &document) :
  m_filterParameter (document.modelFilter().filterParameter()),
  m_intensityLow (document.modelFilter().intensityLow()),
  m_intensityHigh (document.modelFilter().intensityHigh()),
  m_foregroundLow (document.modelFilter().foregroundLow()),
  m_foregroundHigh (document.modelFilter().foregroundHigh()),
  m_hueLow (document.modelFilter().hueLow()),
  m_hueHigh (document.modelFilter().hueHigh()),
  m_saturationLow (document.modelFilter().saturationLow()),
  m_saturationHigh (document.modelFilter().saturationHigh()),
  m_valueLow (document.modelFilter().valueLow()),
  m_valueHigh (document.modelFilter().valueHigh())
{
}

DocumentModelFilter::DocumentModelFilter(const DocumentModelFilter &other) :
  m_filterParameter (other.filterParameter()),
  m_intensityLow (other.intensityLow()),
  m_intensityHigh (other.intensityHigh()),
  m_foregroundLow (other.foregroundLow()),
  m_foregroundHigh (other.foregroundHigh()),
  m_hueLow (other.hueLow()),
  m_hueHigh (other.hueHigh()),
  m_saturationLow (other.saturationLow()),
  m_saturationHigh (other.saturationHigh()),
  m_valueLow (other.valueLow()),
  m_valueHigh (other.valueHigh())
{
}

DocumentModelFilter &DocumentModelFilter::operator=(const DocumentModelFilter &other)
{
  m_filterParameter = other.filterParameter();
  m_intensityLow = other.intensityLow();
  m_intensityHigh = other.intensityHigh();
  m_foregroundLow = other.foregroundLow();
  m_foregroundHigh = other.foregroundHigh();
  m_hueLow = other.hueLow();
  m_hueHigh = other.hueHigh();
  m_saturationLow = other.saturationLow();
  m_saturationHigh = other.saturationHigh();
  m_valueLow = other.valueLow();
  m_valueHigh = other.valueHigh();

  return *this;
}

FilterParameter DocumentModelFilter::filterParameter() const
{
  return m_filterParameter;
}

int DocumentModelFilter::foregroundHigh () const
{
  return m_foregroundHigh;
}

int DocumentModelFilter::foregroundLow () const
{
  return m_foregroundLow;
}

double DocumentModelFilter::high () const
{
  switch (m_filterParameter)
  {
    case FILTER_PARAMETER_FOREGROUND:
      return (double) (m_foregroundHigh - FOREGROUND_MIN) /
          (double) (FOREGROUND_MAX - FOREGROUND_MIN);

    case FILTER_PARAMETER_HUE:
      return (double) (m_hueHigh - HUE_MIN) /
          ((double) HUE_MAX - HUE_MIN);

    case FILTER_PARAMETER_INTENSITY:
      return (double) (m_intensityHigh - INTENSITY_MIN) /
          (double) (INTENSITY_MAX - INTENSITY_MIN);

    case FILTER_PARAMETER_SATURATION:
      return (double) (m_saturationHigh - SATURATION_MIN) /
          (double) (SATURATION_MAX - SATURATION_MIN);

    case FILTER_PARAMETER_VALUE:
      return (double) (m_valueHigh - VALUE_MIN) /
          (double) (VALUE_MAX - VALUE_MIN);

    default:
      Q_ASSERT (false);
  }
}

int DocumentModelFilter::hueHigh () const
{
  return m_hueHigh;
}

int DocumentModelFilter::hueLow () const
{
  return m_hueLow;
}

int DocumentModelFilter::intensityHigh () const
{
  return m_intensityHigh;
}

int DocumentModelFilter::intensityLow () const
{
  return m_intensityLow;
}

double DocumentModelFilter::low () const
{
  switch (m_filterParameter)
  {
    case FILTER_PARAMETER_FOREGROUND:
      return (double) (m_foregroundLow - FOREGROUND_MIN) /
          (double) (FOREGROUND_MAX - FOREGROUND_MIN);

    case FILTER_PARAMETER_HUE:
      return (double) (m_hueLow - HUE_MIN) /
          ((double) HUE_MAX - HUE_MIN);

    case FILTER_PARAMETER_INTENSITY:
      return (double) (m_intensityLow - INTENSITY_MIN) /
          (double) (INTENSITY_MAX - INTENSITY_MIN);

    case FILTER_PARAMETER_SATURATION:
      return (double) (m_saturationLow - SATURATION_MIN) /
          (double) (SATURATION_MAX - SATURATION_MIN);

    case FILTER_PARAMETER_VALUE:
      return (double) (m_valueLow - VALUE_MIN) /
          (double) (VALUE_MAX - VALUE_MIN);

    default:
      Q_ASSERT (false);
  }
}

int DocumentModelFilter::saturationHigh () const
{
  return m_saturationHigh;
}

int DocumentModelFilter::saturationLow () const
{
  return m_saturationLow;
}

void DocumentModelFilter::saveModel(QXmlStreamWriter &stream) const
{
  LOG4CPP_INFO_S ((*mainCat)) << "DocumentModelFilter::saveModel";

  stream.writeStartElement("DocumentModelFilter");
  stream.writeEndElement();
}

void DocumentModelFilter::setFilterParameter(FilterParameter filterParameter)
{
  m_filterParameter = filterParameter;
}

void DocumentModelFilter::setForegroundHigh (int foregroundHigh)
{
  Q_ASSERT (FOREGROUND_MIN <= foregroundHigh && foregroundHigh <= FOREGROUND_MAX);
  m_foregroundHigh = foregroundHigh;
}

void DocumentModelFilter::setForegroundLow (int foregroundLow)
{
  Q_ASSERT (FOREGROUND_MIN <= foregroundLow && foregroundLow <= FOREGROUND_MAX);
  m_foregroundLow = foregroundLow;
}

void DocumentModelFilter::setHigh (double s0To1)
{
  switch (m_filterParameter) {
    case FILTER_PARAMETER_FOREGROUND:
      setForegroundHigh (FOREGROUND_MIN + s0To1 * (FOREGROUND_MAX - FOREGROUND_MIN));
      break;

    case FILTER_PARAMETER_HUE:
      setHueHigh (HUE_MIN + s0To1 * (HUE_MAX - HUE_MIN));
      break;

    case FILTER_PARAMETER_INTENSITY:
      setIntensityHigh (INTENSITY_MIN + s0To1 * (INTENSITY_MAX - INTENSITY_MIN));
      break;

    case FILTER_PARAMETER_SATURATION:
      setSaturationHigh (SATURATION_MIN + s0To1 * (SATURATION_MAX - SATURATION_MIN));
      break;

    case FILTER_PARAMETER_VALUE:
      setValueHigh (VALUE_MIN + s0To1 * (VALUE_MAX - VALUE_MIN));
      break;

    default:
      Q_ASSERT (false);
  }
}

void DocumentModelFilter::setHueHigh (int hueHigh)
{
  Q_ASSERT (HUE_MIN <= hueHigh && hueHigh <= HUE_MAX);
  m_hueHigh = hueHigh;
}

void DocumentModelFilter::setHueLow (int hueLow)
{
  Q_ASSERT (HUE_MIN <= hueLow && hueLow <= HUE_MAX);
  m_hueLow = hueLow;
}

void DocumentModelFilter::setIntensityHigh (int intensityHigh)
{
  Q_ASSERT (INTENSITY_MIN <= intensityHigh && intensityHigh <= INTENSITY_MAX);
  m_intensityHigh = intensityHigh;
}

void DocumentModelFilter::setIntensityLow (int intensityLow)
{
  Q_ASSERT (INTENSITY_MIN <= intensityLow && intensityLow <= INTENSITY_MAX);
  m_intensityLow = intensityLow;
}

void DocumentModelFilter::setLow (double s0To1)
{
  switch (m_filterParameter) {
    case FILTER_PARAMETER_FOREGROUND:
      setForegroundLow (FOREGROUND_MIN + s0To1 * (FOREGROUND_MAX - FOREGROUND_MIN));
      break;

    case FILTER_PARAMETER_HUE:
      setHueLow (HUE_MIN + s0To1 * (HUE_MAX - HUE_MIN));
      break;

    case FILTER_PARAMETER_INTENSITY:
      setIntensityLow (INTENSITY_MIN + s0To1 * (INTENSITY_MAX - INTENSITY_MIN));
      break;

    case FILTER_PARAMETER_SATURATION:
      setSaturationLow (SATURATION_MIN + s0To1 * (SATURATION_MAX - SATURATION_MIN));
      break;

    case FILTER_PARAMETER_VALUE:
      setValueLow (VALUE_MIN + s0To1 * (VALUE_MAX - VALUE_MIN));
      break;

    default:
      Q_ASSERT (false);
  }
}

void DocumentModelFilter::setSaturationHigh (int saturationHigh)
{
  Q_ASSERT (SATURATION_MIN <= saturationHigh && saturationHigh <= SATURATION_MAX);
  m_saturationHigh = saturationHigh;
}

void DocumentModelFilter::setSaturationLow (int saturationLow)
{
  Q_ASSERT (SATURATION_MIN <= saturationLow && saturationLow <= SATURATION_MAX);
  m_saturationLow = saturationLow;
}

void DocumentModelFilter::setValueHigh (int valueHigh)
{
  Q_ASSERT (VALUE_MIN <= valueHigh && valueHigh <= VALUE_MAX);
  m_valueHigh = valueHigh;
}

void DocumentModelFilter::setValueLow (int valueLow)
{
  Q_ASSERT (VALUE_MIN <= valueLow && valueLow <= VALUE_MAX);
  m_valueLow = valueLow;
}

int DocumentModelFilter::valueHigh () const
{
  return m_valueHigh;
}

int DocumentModelFilter::valueLow () const
{
  return m_valueLow;
}
