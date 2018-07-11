#  This file is part of the mantid workbench.
#
#  Copyright (C) 2018 mantidproject
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
from __future__ import absolute_import, print_function

import re

from qtpy.QtCore import Qt, Signal, QMutex, QMutexLocker
from qtpy.QtWidgets import (QAbstractItemView, QAction, QActionGroup, QFileDialog, QHBoxLayout, QLineEdit, QMenu,
                            QPushButton, QTableWidget, QTableWidgetItem, QVBoxLayout, QWidget)

import qtawesome as qta

from mantidqt.utils.flowlayout import FlowLayout
from mantidqt.py3compat import Enum
from workbench.plotting.qappthreadcall import QAppThreadCall

DEBUG_MODE = False

EXPORT_TYPES = [
    ('Export to EPS', '.eps'),
    ('Export to PDF', '.pdf'),
    ('Export to PNG', '.png'),
    ('Export to SVG', '.svg')
]


class Column(Enum):
    Number = 0
    Name = 1
    LastActive = 2


class PlotSelectorView(QWidget):
    """
    The view to the plot selector, a PyQt widget.
    """

    # A signal to capture when keys are pressed
    deleteKeyPressed = Signal(int)
    enterKeyPressed = Signal(int)

    def __init__(self, presenter, parent=None, is_run_as_unit_test=False):
        """
        Initialise a new instance of PlotSelectorWidget
        :param presenter: The presenter controlling this view
        :param parent: Optional - the parent QWidget
        :param is_run_as_unit_test: Optional - True if this is
        running as a unit test, in which case skip file dialogs
        """
        super(PlotSelectorView, self).__init__(parent)
        self.presenter = presenter
        self.is_run_as_unit_test = is_run_as_unit_test

        # This mutex prevents multiple operations on the table at the
        # same time. Wrap code in - with QMutexLocker(self.mutex):
        self.mutex = QMutex()

        self.sort_order = Qt.AscendingOrder
        self.sort_type = Column.Name

        self.show_button = QPushButton('Show')
        self.select_all_button = QPushButton('Select All')
        self.close_button = QPushButton('Close')
        self.sort_button = self._make_sort_button()
        self.export_button = self._make_export_button()
        self.filter_box = self._make_filter_box()
        self.table_widget = self._make_table_widget()

        # Add the context menu
        self.table_widget.setContextMenuPolicy(Qt.CustomContextMenu)
        self.context_menu, self.export_menu = self._make_context_menu()
        self.table_widget.customContextMenuRequested.connect(self.context_menu_opened)

        buttons_layout = FlowLayout()
        buttons_layout.setSpacing(1)
        buttons_layout.addWidget(self.show_button)
        buttons_layout.addWidget(self.select_all_button)
        buttons_layout.addWidget(self.close_button)
        buttons_layout.addWidget(self.sort_button)
        buttons_layout.addWidget(self.export_button)

        filter_layout = QHBoxLayout()
        filter_layout.addWidget(self.filter_box)

        layout = QVBoxLayout()
        layout.addLayout(buttons_layout)
        layout.addLayout(filter_layout)
        layout.addWidget(self.table_widget)
        # todo: Without the sizeHint() call the minimum size is not set correctly
        #       This needs some investigation as to why this is.
        layout.sizeHint()
        self.setLayout(layout)

        # These must happen in the main GUI thread, else segfaults
        self.set_plot_list_orig = self.set_plot_list
        self.set_plot_list = QAppThreadCall(self.set_plot_list_orig)
        self.append_to_plot_list_orig = self.append_to_plot_list
        self.append_to_plot_list = QAppThreadCall(self.append_to_plot_list_orig)
        self.remove_from_plot_list_orig = self.remove_from_plot_list
        self.remove_from_plot_list = QAppThreadCall(self.remove_from_plot_list_orig)
        self.rename_in_plot_list_orig = self.rename_in_plot_list
        self.rename_in_plot_list = QAppThreadCall(self.rename_in_plot_list_orig)

        # Connect presenter methods to things in the view
        self.table_widget.doubleClicked.connect(self.presenter.show_single_selected)
        self.filter_box.textChanged.connect(self.presenter.filter_text_changed)
        self.show_button.clicked.connect(self.presenter.show_multiple_selected)
        self.select_all_button.clicked.connect(self.table_widget.selectAll)
        self.close_button.clicked.connect(self.presenter.close_action_called)
        self.deleteKeyPressed.connect(self.presenter.close_action_called)

        if DEBUG_MODE:
            self.table_widget.clicked.connect(self.show_debug_info)

    def show_debug_info(self):
        """
        Special feature to make debugging easier, set DEBUG_MODE to
        true to get information printed when clicking on plots
        """
        row = self.table_widget.currentRow()
        widget = self.table_widget.cellWidget(row, Column.Name)
        print("Plot number: {}".format(widget.plot_number))
        print("Plot text: {}".format(widget.line_edit.text()))

    def keyPressEvent(self, event):
        """
        This overrides keyPressEvent from QWidget to emit a signal
        whenever the delete key is pressed.

        This might be better to override on table_widget, but there is
        only ever an active selection when focused on the table.
        :param event: A QKeyEvent holding the key that was pressed
        """
        super(PlotSelectorView, self).keyPressEvent(event)
        if event.key() == Qt.Key_Delete:
            self.deleteKeyPressed.emit(event.key())

    def _make_table_widget(self):
        """
        Make a table showing the matplotlib figure number of the
        plots, the name with close and edit buttons, and a hidden
        column for sorting with the last actuve order
        :return: A QTableWidget object which will contain plot widgets
        """
        table_widget = QTableWidget(0, 3, self)
        table_widget.setHorizontalHeaderLabels(['No.', 'Plot Name', 'Last Active Order (hidden)'])
        if not DEBUG_MODE:
            table_widget.setColumnHidden(Column.LastActive, True)
        table_widget.verticalHeader().setVisible(False)

        # Fix the size of 'No.' and let 'Plot Name' fill the space
        top_header = table_widget.horizontalHeader()
        top_header.resizeSection(Column.Number, top_header.sectionSizeHint(Column.Number))
        top_header.setStretchLastSection(True)

        table_widget.horizontalHeaderItem(Column.Number).setToolTip('This is the matplotlib figure number.\n\n'
                 'From a script use plt.figure(N), where N is this figure number, to get a handle to the plot.')

        table_widget.horizontalHeaderItem(Column.Name).setToolTip('The plot name, also used  as the file name when '
                                                                  'saving multiple plots.')

        table_widget.setSelectionBehavior(QAbstractItemView.SelectRows)
        table_widget.setSelectionMode(QAbstractItemView.ExtendedSelection)
        table_widget.setEditTriggers(QAbstractItemView.NoEditTriggers)
        table_widget.setSortingEnabled(True)

        return table_widget

    def _make_context_menu(self):
        """
        Makes the context menu with options relating to plots
        :return: The context menu, and export sub-menu with a list of
                 export types
        """
        context_menu = QMenu()
        context_menu.addAction("Show", self.presenter.show_multiple_selected)
        context_menu.addAction("Rename", self.rename_selected_in_context_menu)

        export_menu = context_menu.addMenu("Export")
        for text, extension in EXPORT_TYPES:
            export_menu.addAction(text, lambda ext=extension: self.export_plots(ext))

        context_menu.addAction("Close", self.presenter.close_action_called)
        return context_menu, export_menu

    def context_menu_opened(self, position):
        """
        Open the context menu in the correct location
        :param position: The position to open the menu, e.g. where
                         the mouse button was clicked
        """
        self.context_menu.exec_(self.table_widget.mapToGlobal(position))

    # ------------------------ Plot Updates ------------------------

    def append_to_plot_list(self, plot_number, is_shown_by_filter):
        """
        Appends to the plot list, if sorting is enabled this should
        automatically go to the correct place, and the flag can be
        set to determine whether it should initially be hidden or not
        :param plot_number: The unique number in GlobalFigureManager
        :param is_shown_by_filter: If true then show this plot name
                                   in the list, else hide it
        """
        # TODO: move is_shown_by_filter to presernter logic

        plot_name_widget = PlotNameWidget(self.presenter, plot_number, self, self.is_run_as_unit_test)

        number_item = HumanReadableSortItem(str(plot_number))
        number_item.setTextAlignment(Qt.AlignRight | Qt.AlignVCenter)

        name_item = HumanReadableSortItem()
        name_item.setSizeHint(plot_name_widget.sizeHint())

        last_active_item = HumanReadableSortItem()
        last_active_value = self.presenter.get_initial_last_active_value(plot_number)
        last_active_item.setData(Qt.DisplayRole, last_active_value)

        with QMutexLocker(self.mutex):
            self.table_widget.setSortingEnabled(False)
            row_number = self.table_widget.rowCount()
            self.table_widget.insertRow(row_number)

            self.table_widget.setItem(row_number, Column.Number, number_item)

            self.table_widget.setItem(row_number, Column.Name, name_item)
            self.table_widget.setCellWidget(row_number, Column.Name, plot_name_widget)

            self.table_widget.setItem(row_number, Column.LastActive, last_active_item)

            self.table_widget.setRowHidden(row_number, not is_shown_by_filter)
            self.table_widget.setSortingEnabled(True)

    def set_plot_list(self, plot_list):
        """
        Populate the plot list from the Presenter. This is reserved
        for a 'things have gone wrong' scenario, and should only be
        used when errors are encountered.
        :param plot_list: the list of plot numbers
        """
        with QMutexLocker(self.mutex):
            self.table_widget.clearContents()

        self.filter_box.clear()
        for plot_number in plot_list:
            plot_name = self.presenter.get_plot_name_from_number(plot_number)
            self.append_to_plot_list(plot_name, True)

    def _get_row_and_widget_from_plot_number(self, plot_number):
        """
        Get the row in the table, and the PlotNameWidget corresponding
        to the given the plot name. This should always be called with
        a lock on self.mutex.
        :param plot_number: The unique number in GlobalFigureManager
        """
        for row in range(self.table_widget.rowCount()):
            widget = self.table_widget.cellWidget(row, Column.Name)
            if widget.plot_number == plot_number:
                return row, widget

    def remove_from_plot_list(self, plot_number):
        """
        Remove the given plot name from the list
        :param plot_number: The unique number in GlobalFigureManager
        """
        with QMutexLocker(self.mutex):
            row, widget = self._get_row_and_widget_from_plot_number(plot_number)
            self.table_widget.removeRow(row)

    # ----------------------- Plot Selection ------------------------

    def get_all_selected_plot_numbers(self):
        """
        Returns a list with the numbers of all the currently selected
        plots
        :return: A list of strings with the plot numbers
        """
        selected = self.table_widget.selectedItems()
        selected_plots = []
        for item in selected:
            row = item.row()
            if not self.table_widget.isRowHidden(row):
                selected_plots.append(self.table_widget.cellWidget(row, Column.Name).plot_number)
        return selected_plots

    def get_currently_selected_plot_number(self):
        """
        Returns a string with the plot number for the currently
        active plot
        :return: A string with the plot number
        """
        row = self.table_widget.currentRow()
        if self.table_widget.isRowHidden(row):
            return None
        return self.table_widget.cellWidget(row, Column.Name).plot_number

    def get_filter_text(self):
        """
        Returns the currently set text in the filter box
        :return: A string with current filter text
        """
        return self.filter_box.text()

    # ----------------------- Plot Filtering ------------------------

    def _make_filter_box(self):
        """
        Make the text box to filter the plots by name
        :return: A QLineEdit object with placeholder text and a
                 clear button
        """
        text_box = QLineEdit(self)
        text_box.setPlaceholderText("Filter Plots")
        text_box.setClearButtonEnabled(True)
        return text_box

    def unhide_all_plots(self):
        """
        Set all plot names to be visible (not hidden)
        """
        with QMutexLocker(self.mutex):
            for row in range(self.table_widget.rowCount()):
                self.table_widget.setRowHidden(row, False)

    def filter_plot_list(self, plot_list):
        """
        Given a list of plots numbers, show only the ones in the list,
        hiding the rest
        :param plot_list: The list of plots to show
        """
        # TODO: work correctly with numbers
        with QMutexLocker(self.mutex):
            for row in range(self.table_widget.rowCount()):
                widget = self.table_widget.cellWidget(row, Column.Name)
                self.table_widget.setRowHidden(row, widget.plot_name not in plot_list)

    # ------------------------ Plot Renaming ------------------------

    def rename_in_plot_list(self, plot_number, new_name):
        """
        Rename a plot in the plot list, also setting the sort key
        :param plot_number: The unique number in GlobalFigureManager
        :param new_name: The new plot name
          """
        with QMutexLocker(self.mutex):
            row, widget = self._get_row_and_widget_from_plot_number(plot_number)

            old_key = self.table_widget.item(row, Column.LastActive).data(Qt.DisplayRole)
            new_sort_key = self.presenter.get_renamed_last_active_value(plot_number, old_key)
            self.table_widget.item(row, Column.LastActive).setData(Qt.InitialSortOrderRole, new_sort_key)

            widget.set_plot_name(new_name)

    def rename_selected_in_context_menu(self):
        """
        Triggered when rename is selected from the context menu,
        makes the plot name directly editable
        """
        plot_number = self.get_currently_selected_plot_number()
        with QMutexLocker(self.mutex):
            row, widget = self._get_row_and_widget_from_plot_number(plot_number)
        widget.toggle_plot_name_editable(True)

    # ----------------------- Plot Sorting --------------------------
    """
    How the Sorting Works
    
    Sorting acts on the three columns for plot number, plot name and
    last active order. Last active order is a hidden column, not
    intended for the user to see, but can be set from the sort menu
    button.

    If sorting by last active this is the number the plot was last
    active, or if never active it is the plot name with an '_'
    appended to the front. For example ['1', '2', '_UnshownPlot'].

    QTableWidgetItem is subclassed by HumanReadableSortItem to
    override the < operator. This uses the text with the
    InitialSortOrderRole to sort, and sorts in a human readable way,
    for example ['Figure 1', 'Figure 2', 'Figure 10'] as opposed to
    the ['Figure 1', 'Figure 10', 'Figure 2'].
    """

    def _make_sort_button(self):
        """
        Make the sort button, with separate groups for ascending and
        descending, sorting by name or last shown
        :return: The sort menu button
        """
        sort_button = QPushButton("Sort")
        sort_menu = QMenu()

        ascending_action = QAction("Ascending", sort_menu, checkable=True)
        ascending_action.setChecked(True)
        ascending_action.toggled.connect(self.presenter.set_sort_order)
        descending_action = QAction("Descending", sort_menu, checkable=True)

        order_group = QActionGroup(sort_menu)
        order_group.addAction(ascending_action)
        order_group.addAction(descending_action)

        number_action = QAction("Number", sort_menu, checkable=True)
        number_action.setChecked(True)
        number_action.toggled.connect(lambda: self.presenter.set_sort_type(Column.Number))
        name_action = QAction("Name", sort_menu, checkable=True)
        name_action.toggled.connect(lambda: self.presenter.set_sort_type(Column.Name))
        last_active_action = QAction("Last Active", sort_menu, checkable=True)
        last_active_action.toggled.connect(lambda: self.presenter.set_sort_type(Column.LastActive))

        sort_type_group = QActionGroup(sort_menu)
        sort_type_group.addAction(number_action)
        sort_type_group.addAction(name_action)
        sort_type_group.addAction(last_active_action)

        sort_menu.addAction(ascending_action)
        sort_menu.addAction(descending_action)
        sort_menu.addSeparator()
        sort_menu.addAction(number_action)
        sort_menu.addAction(name_action)
        sort_menu.addAction(last_active_action)

        sort_button.setMenu(sort_menu)
        return sort_button

    def sort_ascending(self):
        """
        Set the list to sort ascending

        See also HumanReadableSortItem class
        """
        self.sort_order = Qt.AscendingOrder
        with QMutexLocker(self.mutex):
            self.table_widget.sortItems(Column.Name, self.sort_order)

    def sort_descending(self):
        """
        Set the list to sort descending

        See also HumanReadableSortItem class
        """
        self.sort_order = Qt.DescendingOrder
        with QMutexLocker(self.mutex):
            self.table_widget.sortItems(Column.Name, self.sort_order)

    def set_sort_type(self, sort_type):
        """
        Set sorting to be by name - note this does not update any
        sort keys, it just sets the required state
        :param sort_type: A Column enum for the column to sort on
        """
        self.sort_type = sort_type
        with QMutexLocker(self.mutex):
            self.table_widget.sortItems(sort_type, self.sort_order)

    def set_last_active_values(self, last_active_values):
        """
        Sets the sort keys given a dictionary of plot numbers and
        last active values, e.g. {1: 2, 2: 1, 7: 3}
        :param last_active_values: A dictionary with keys as plot
                                   number and values as last active
                                   order
        """
        with QMutexLocker(self.mutex):
            self.table_widget.setSortingEnabled(False)
            for row in range(self.table_widget.rowCount()):
                plot_number = self.table_widget.cellWidget(row, Column.Name).plot_number

                last_active_item = self.table_widget.item(row, Column.LastActive)
                last_active_value = last_active_values.get(plot_number)

                if last_active_value:
                    last_active_item.setData(Qt.DisplayRole, str(last_active_value))

            self.table_widget.sortItems(Column.LastActive, self.sort_order)
            self.table_widget.setSortingEnabled(True)

    # ---------------------- Plot Exporting -------------------------

    def _make_export_button(self):
        """
        Makes the export button menu, containing a list of export
        types
        :return: The export button menu
        """
        export_button = QPushButton("Export")
        export_menu = QMenu()
        for text, extension in EXPORT_TYPES:
            export_menu.addAction(text, lambda ext=extension: self.export_plots(ext))
        export_button.setMenu(export_menu)
        return export_button

    def export_plots(self, extension):
        """
        Pops up a directory selection dialogue then calls the
        presenter to do the export - note that if this is run as a
        unit test this directory selection must be disabled, else it
        actually appears!
        :param extension: The file extension to use which defines the
                          export type
        """
        path = ""
        if not self.is_run_as_unit_test:
            path = QFileDialog.getExistingDirectory(None, 'Select folder for exported plots')
        self.presenter.export_plots(path, extension)


class PlotNameWidget(QWidget):
    """A widget to display the plot name, and edit and close buttons

    This widget is added to the table widget to support the renaming
    and close buttons, as well as the direct renaming functionality.
    """
    def __init__(self, presenter, plot_number, parent=None, is_run_as_unit_test=False):
        super(PlotNameWidget, self).__init__(parent)

        self.presenter = presenter
        self.plot_number = plot_number
        self.is_run_as_unit_test = is_run_as_unit_test

        self.line_edit = QLineEdit(self.presenter.get_plot_name_from_number(plot_number))
        self.line_edit.setReadOnly(True)
        self.line_edit.setFrame(False)
        self.line_edit.setStyleSheet("* { background-color: rgba(0, 0, 0, 0); }")
        self.line_edit.setAttribute(Qt.WA_TransparentForMouseEvents, True)
        self.line_edit.editingFinished.connect(self.rename_plot)

        rename_icon = qta.icon('fa.edit')
        self.rename_button = QPushButton(rename_icon, "")
        self.rename_button.setFlat(True)
        self.rename_button.setMaximumWidth(self.rename_button.iconSize().width() * 2)
        self.rename_button.setCheckable(True)
        self.rename_button.toggled.connect(self.rename_button_toggled)
        self.skip_next_toggle = False

        close_icon = qta.icon('fa.close')
        self.close_button = QPushButton(close_icon, "")
        self.close_button.setFlat(True)
        self.close_button.setMaximumWidth(self.close_button.iconSize().width() * 2)
        self.close_button.clicked.connect(lambda: self.close_pressed(self.plot_number))

        self.layout = QHBoxLayout()

        # Get rid of the top and bottom margins - the button provides
        # some natural margin anyway. Get rid of right margin and
        # reduce spacing to get buttons closer together.
        self.layout.setContentsMargins(5, 0, 0, 0)
        self.layout.setSpacing(0)

        self.layout.addWidget(self.line_edit)
        self.layout.addWidget(self.rename_button)
        self.layout.addWidget(self.close_button)

        self.layout.sizeHint()
        self.setLayout(self.layout)

    def set_plot_name(self, new_name):
        """
        Sets the internally stored and displayed plot name
        :param new_name: The name to set
        """
        self.line_edit.setText(new_name)

    def close_pressed(self, plot_number):
        """
        Close the plot with the given name
        :param plot_number: The unique number in GlobalFigureManager
        """
        self.presenter.close_single_plot(plot_number)

    def rename_button_toggled(self, checked):
        """
        If the rename button is pressed from being unchecked then
        make the line edit item editable
        :param checked: True if the rename toggle is now pressed
        """
        if checked:
            self.toggle_plot_name_editable(True, toggle_rename_button=False)

    def toggle_plot_name_editable(self, editable, toggle_rename_button=True):
        """
        Set the line edit item to be editable or not editable. If
        editable move the cursor focus to the editable name and
        highlight it all.
        :param editable: If true make the plot name editable, else
                         make it read only
        :param toggle_rename_button: If true also toggle the rename
                                     button state
        """
        self.line_edit.setReadOnly(not editable)
        self.line_edit.setAttribute(Qt.WA_TransparentForMouseEvents, not editable)

        # This is a sneaky way to avoid the issue of two calls to
        # this toggle method, by effectively disabling the button
        # press in edit mode.
        self.rename_button.setAttribute(Qt.WA_TransparentForMouseEvents, editable)

        if toggle_rename_button:
            self.rename_button.setChecked(editable)

        if editable:
            self.line_edit.setFocus()
            self.line_edit.selectAll()
        else:
            self.line_edit.setSelection(0, 0)

    def rename_plot(self):
        """
        Called when the editing is finished, gets the presenter to
        do the real renaming of the plot
        """
        self.presenter.rename_figure(self.plot_number, self.line_edit.text())
        self.toggle_plot_name_editable(False)


class HumanReadableSortItem(QTableWidgetItem):
    """Inherits from QTableWidgetItem and override __lt__ method

    This overrides the  < operator (or __lt__ method) to make a human
    readable sort, for example
      [Figure 1, Figure 2, Figure 3, Figure 20]
    instead of
      [Figure 1, Figure 2, Figure 20, Figure 3]
    """

    def convert(self, text):
        """
        Convert some text for comparison
        :param text: A string with the text to convert
        :return: An int if the string is fully numeric, else the text
                 in lower case
        """
        return int(text) if text.isdigit() else text.lower()

    def __lt__(self, other):
        """
        Override the less than method to do a human readable sort.
        Essentially break the string up in the following way:
         'Plot1 Version23' -> ['Plot', 1, 'Version', 23]
        then uses Python list comparison to get the result we want.
        :param other: The other HumanReadableSortItem to compare with
        """
        self_key = [self.convert(c) for c in re.split('([0-9]+)', str(self.data(Qt.InitialSortOrderRole)))]
        other_key = [self.convert(c) for c in re.split('([0-9]+)', str(other.data(Qt.InitialSortOrderRole)))]
        return self_key < other_key
