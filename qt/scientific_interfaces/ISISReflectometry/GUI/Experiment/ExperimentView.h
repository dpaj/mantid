// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2016 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_CUSTOMINTERFACES_EXPERIMENTVIEW_H_
#define MANTID_CUSTOMINTERFACES_EXPERIMENTVIEW_H_

#include "Common/DllConfig.h"
#include "IExperimentView.h"
#include "MantidQtWidgets/Common/HintingLineEdit.h"
#include "ui_ExperimentWidget.h"
#include <QCheckBox>
#include <QShortcut>
#include <memory>

namespace MantidQt {
namespace CustomInterfaces {

/** ExperiementView : Provides an interface for the "Experiement" tab in the
ISIS Reflectometry interface.
*/
class MANTIDQT_ISISREFLECTOMETRY_DLL ExperimentView : public QWidget,
                                                      public IExperimentView {
  Q_OBJECT
public:
  ExperimentView(Mantid::API::IAlgorithm_sptr algorithmForTooltips,
                 QWidget *parent = nullptr);
  void subscribe(ExperimentViewSubscriber *notifyee) override;
  void connectExperimentSettingsWidgets() override;
  void disconnectExperimentSettingsWidgets() override;

  void
  createStitchHints(const std::vector<MantidWidgets::Hint> &hints) override;

  std::string getAnalysisMode() const override;
  void setAnalysisMode(std::string const &analysisMode) override;

  std::string getSummationType() const override;
  void setSummationType(std::string const &summationType) override;

  std::string getReductionType() const override;
  void setReductionType(std::string const &reductionType) override;
  void enableReductionType() override;
  void disableReductionType() override;

  bool getIncludePartialBins() const override;
  void setIncludePartialBins(bool enable) override;
  void enableIncludePartialBins() override;
  void disableIncludePartialBins() override;

  bool getDebugOption() const override;
  void setDebugOption(bool enable) override;

  std::vector<std::array<std::string, 8>> getPerAngleOptions() const override;
  void
      setPerAngleOptions(std::vector<std::array<std::string, 8>> rows) override;
  void showPerAngleOptionsAsInvalid(int row, int column) override;
  void showPerAngleOptionsAsValid(int row) override;
  void showPerAngleThetasNonUnique(double thetaTolerance) override;
  void showStitchParametersValid() override;
  void showStitchParametersInvalid() override;

  double getTransmissionStartOverlap() const override;
  void setTransmissionStartOverlap(double start) override;
  double getTransmissionEndOverlap() const override;
  void setTransmissionEndOverlap(double end) override;
  void showTransmissionRangeInvalid() override;
  void showTransmissionRangeValid() override;

  std::string getPolarizationCorrectionType() const override;
  void setPolarizationCorrectionType(std::string const &type) override;
  double getCRho() const override;
  void setCRho(double cRho) override;
  double getCAlpha() const override;
  void setCAlpha(double cAlpha) override;
  double getCAp() const override;
  void setCAp(double cAp) override;
  double getCPp() const override;
  void setCPp(double cPp) override;

  std::string getFloodCorrectionType() const override;
  void setFloodCorrectionType(std::string const &correction) override;
  std::string getFloodWorkspace() const override;
  void setFloodWorkspace(std::string const &workspace) override;

  std::string getStitchOptions() const override;
  void setStitchOptions(std::string const &stitchOptions) override;

  void showOptionLoadErrors(
      std::vector<InstrumentParameterTypeMissmatch> const &typeErrors,
      std::vector<MissingInstrumentParameterValue> const &missingValues)
      override;

  void showAllPerAngleOptionsAsValid() override;

  void disableAll() override;
  void enableAll() override;

  void enablePolarizationCorrections() override;
  void disablePolarizationCorrections() override;
  void enablePolarizationCorrectionInputs() override;
  void disablePolarizationCorrectionInputs() override;

  void enableFloodCorrectionInputs() override;
  void disableFloodCorrectionInputs() override;

  void addPerThetaDefaultsRow() override;
  void removePerThetaDefaultsRow(int rowIndex) override;

public slots:
  /// Adds another row to the per-angle options table
  void onRestoreDefaultsRequested();
  void onSummationTypeChanged(int reductionTypeIndex);
  void onNewPerThetaDefaultsRowRequested();
  void onRemovePerThetaDefaultsRequested();
  void onSettingsChanged();
  void onPerAngleDefaultsChanged(int row, int column);

private:
  void initializeTableItems(QTableWidget &table);
  void initializeTableRow(QTableWidget &table, int row);
  void initializeTableRow(QTableWidget &table, int row,
                          std::array<std::string, 8> rowValues);
  QString messageFor(
      std::vector<MissingInstrumentParameterValue> const &missingValues) const;
  QString messageFor(const InstrumentParameterTypeMissmatch &typeError) const;

  /// Initialise the interface
  void initLayout();
  void initOptionsTable();
  void initFloodControls();
  void registerSettingsWidgets(Mantid::API::IAlgorithm_sptr alg);
  void registerExperimentSettingsWidgets(Mantid::API::IAlgorithm_sptr alg);
  void setToolTipAsPropertyDocumentation(QWidget &widget,
                                         std::string const &propertyName,
                                         Mantid::API::IAlgorithm_sptr alg);

  template <typename Widget>
  void registerSettingWidget(Widget &widget, std::string const &propertyName,
                             Mantid::API::IAlgorithm_sptr alg);
  void connectSettingsChange(QLineEdit &edit);
  void connectSettingsChange(QComboBox &edit);
  void connectSettingsChange(QCheckBox &edit);
  void connectSettingsChange(QTableWidget &edit);
  void connectSettingsChange(QDoubleSpinBox &edit);
  void disconnectSettingsChange(QLineEdit &edit);
  void disconnectSettingsChange(QComboBox &edit);
  void disconnectSettingsChange(QCheckBox &edit);
  void disconnectSettingsChange(QTableWidget &edit);
  void disconnectSettingsChange(QDoubleSpinBox &edit);
  QLineEdit &stitchOptionsLineEdit() const;
  void setSelected(QComboBox &box, std::string const &str);
  void setText(QLineEdit &lineEdit, int value);
  void setText(QLineEdit &lineEdit, double value);
  void setText(QLineEdit &lineEdit, std::string const &value);
  void setText(QLineEdit &lineEdit, boost::optional<int> value);
  void setText(QLineEdit &lineEdit, boost::optional<double> value);
  void setText(QLineEdit &lineEdit, boost::optional<std::string> const &value);
  std::string textFromCell(QTableWidgetItem const *maybeNullItem) const;
  //  void setText(QTableWidget &table, std::string const &propertyName,
  //               double value);
  //  void setText(QTableWidget &table, std::string const &propertyName,
  //               boost::optional<double> value);
  //  void setText(QTableWidget &table, std::string const &propertyName,
  //               boost::optional<std::string> value);
  //  void setText(QTableWidget &table, std::string const &propertyName,
  //               std::string const &value);
  //  void setText(QTableWidget &table, std::string const &propertyName,
  //               const QString &value);
  void setChecked(QCheckBox &checkBox, bool checked);
  std::string getText(QLineEdit const &lineEdit) const;
  std::string getText(QComboBox const &box) const;
  void setEnabledStateForAllWidgets(bool enabled);
  /// The stitch params entry widget
  MantidQt::MantidWidgets::HintingLineEdit *m_stitchEdit;

  std::unique_ptr<QShortcut> m_deleteShortcut;
  Ui::ExperimentWidget m_ui;
  ExperimentViewSubscriber *m_notifyee;
};

} // namespace CustomInterfaces
} // namespace MantidQt

#endif /* MANTID_CUSTOMINTERFACES_EXPERIMENTVIEW_H_ */
