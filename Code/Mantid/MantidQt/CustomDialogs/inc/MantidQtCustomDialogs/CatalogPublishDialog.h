#ifndef MANTIDQT_CUSTOM_DIALOGS_CATALOGPUBLISHDIALOG_H
#define MANTIDQT_CUSTOM_DIALOGS_CATALOGPUBLISHDIALOG_H

#include "MantidQtAPI/AlgorithmDialog.h"
#include "ui_CatalogPublishDialog.h"

namespace MantidQt
{
  namespace CustomDialogs
  {
    class CatalogPublishDialog : public MantidQt::API::AlgorithmDialog
    {
      Q_OBJECT

      public:
        /// Constructor
        CatalogPublishDialog(QWidget *parent = 0);
        /// Destructor
        ~CatalogPublishDialog();

      private:
        /// Create the inital layout.
        virtual void initLayout();
        /// Populate the investigation number combo-box with investigations that the user can publish to.
        void populateUserInvestigations();

      protected:
        /// The form generated by QT Designer.
        Ui::CatalogPublishDialog m_uiForm;
    };
  }
}

#endif /* MANTIDQT_CUSTOM_DIALOGS_CATALOGPUBLISHDIALOG_H */
