#ifndef MANTIDQT_CUSTOMINTERFACES_ENGGDIFFMULTIRUNFITTINGQTWIDGET_H_
#define MANTIDQT_CUSTOMINTERFACES_ENGGDIFFMULTIRUNFITTINGQTWIDGET_H_

#include "DllConfig.h"
#include "IEnggDiffMultiRunFittingWidgetPresenter.h"
#include "IEnggDiffMultiRunFittingWidgetView.h"
#include "IEnggDiffractionUserMsg.h"
#include "ui_EnggDiffMultiRunFittingWidget.h"

#include <boost/shared_ptr.hpp>
#include <qwt_plot_curve.h>

namespace MantidQt {
namespace CustomInterfaces {

class MANTIDQT_ENGGDIFFRACTION_DLL EnggDiffMultiRunFittingQtWidget
    : public QWidget,
      public IEnggDiffMultiRunFittingWidgetView {
  Q_OBJECT

public:
  EnggDiffMultiRunFittingQtWidget(
      boost::shared_ptr<IEnggDiffMultiRunFittingWidgetPresenter> presenter,
      boost::shared_ptr<IEnggDiffractionUserMsg> messageProvider);

  ~EnggDiffMultiRunFittingQtWidget() override;

  std::pair<int, size_t> getSelectedRunLabel() const override;

  void plotFittedPeaks(
      const std::vector<boost::shared_ptr<QwtData>> &curve) override;

  void
  plotFocusedRun(const std::vector<boost::shared_ptr<QwtData>> &curve) override;

  void resetCanvas() override;

  bool showFitResultsSelected() const override;

  void
  updateRunList(const std::vector<std::pair<int, size_t>> &runLabels) override;

  void userError(const std::string &errorTitle,
                 const std::string &errorDescription) override;

private slots:
  void processSelectRun();

private:
  void cleanUpPlot();

  void setupUI();

  std::vector<std::unique_ptr<QwtPlotCurve>> m_focusedRunCurves;

  boost::shared_ptr<IEnggDiffMultiRunFittingWidgetPresenter> m_presenter;

  Ui::EnggDiffMultiRunFittingWidget m_ui;

  boost::shared_ptr<IEnggDiffractionUserMsg> m_userMessageProvider;
};

} // CustomInterfaces
} // MantidQt

#endif // MANTIDQT_CUSTOMINTERFACES_ENGGDIFFMULTIRUNFITTINGQTWIDGET_H_
