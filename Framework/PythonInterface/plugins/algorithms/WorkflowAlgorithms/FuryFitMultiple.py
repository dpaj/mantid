#pylint: disable=no-init
from mantid import config, logger, AlgorithmFactory
from mantid.api import *
from mantid.kernel import *
from mantid.simpleapi import *
import os.path

class FuryFitMultiple(PythonAlgorithm):

    _inputWs
    _function
    _fit_type
    _start_x
    _end_x
    _spec_min
    _spec_max
    _intensities_constrained
    _minimizer
    _max_iterations
    _save
    _plot

    def category(self):
        return "Workflow\\MIDAS"

    def summary(self):
        #pylint: disable=anomalous-backslash-in-string
        return "Fits an \*\_iqt file generated by I(Q,t)."

    def PyInit(self):
        self.declareProperty(MatrixWorkspaceProperty('InputWorkspace', '', direction=Direction.Input),
                             doc='The _iqt.nxs InputWorkspace used by the algorithm')
        self.declareProperty(name='Function', '',
                             doc='The function to use in fitting')
        self.declareProperty(name='FitType', '',
                             doc='The type of fit being carried out')
        self.declareProperty(name='StartX', defaultValue=0,
                             doc="The first value for X")
        self.declareProperty(name='EndX', '',
                             doc="The last value for X")
        self.declareProperty(name='SpecMin', defaultValue=0,
                             doc='Minimum spectra in the worksapce to fit')
        self.declareProperty(name='SpecMax', '',
                             doc='Maximum spectra in the worksapce to fit')
        self.declareProperty(name='Minimizer','Levenberg-Marquardt',
                             doc='The minimizer to use in fitting')
        self.declareProperty(name="MaxIterations", defaultValue=500,
                             doc="The Maximum number of iterations for the fit")
        self.declareProperty(name='ConstrainIntensities', defaultValue=False,
                             doc="If the Intensities should be constrained during the fit")
        self.declareProperty(name='Save', defaultValue=False,
                             doc="Should the Output of the algorithm be saved to working directory")
        self.declareProperty(name='Plot', defaultValue='None', validator=StringListValidator(['None', 'Intensity', 'Tau', 'Beta', 'All']),
                             doc='Switch Plot Off/On')


    def validateInputs(self):
        self._get_properties()
        issues = dict()

        input_workspace = mtd[self._input_ws]
        maximum_possible_spectra = input_workspace.getNumberhistograms()
        maximum_possible_x = input_workspace.readX(0)[input_workspace.blocksize()]
        # Validate SpecMin/Max

        if self._spec_max > maximum_possible_spectra:
            issues['SpecMax'] = ('SpecMax must be smaller or equal to the number of spectra in the input workspace, %d' % maximum_possible_spectra)
        if self._spec_min < 0:
            issues['SpecMin'] = 'SpecMin can not be less than 0'
        if self._spec_max < self.spec_min:
            issues['SpecMax'] = 'SpecMax must be more than or equal to SpecMin'

        # Validate Start/EndX
        if self._end_x > maximum_possible_x:
            issues['EndX'] = ('EndX must be less than the highest x value in the workspace, %d' % maximum_possible_x)
        if self._start_x < 0:
            issues['StartX'] = 'StartX can not be less than 0'
        if self._start_x < self._end_x:
            issues['EndX'] = 'EndX must be more than StartX'

        return issues

    def _get_properties(self):
        self._input_ws = self.getProperty('InputWorkspace').value
        self._function = self.getProperty('Function').value
        self._fit_type = self.getProperty('FitType').value
        self._start_x = self.getProperty('StartX').value
        self._end_x = self.getProperty('EndX').value
        self._spec_min = self.getProperty('SpecMin').value
        self._spec_max = self.getProperty('SpecMax').value
        self._intensities_constrained = self.getProperty('ConstrainIntensities').value
        self._minimizer = self.getProperty('Minimizer').value
        self._max_iterations = self.getProperty('MaxIterations').value
        self._save = self.getProperty('Save').value
        self._plot = self.getProperty('Plot').value

    def PyExec(self):
        from IndirectDataAnalysis import furyfitMult

        # Run FuryFitMultiple algorithm from indirectDataAnalysis
        furyfitMult(self._input_ws, self._function, self._fit_type, self._start_x, self._end_x,
                    self._spec_min, self._spec_max, self._intensities_constrained,
                    self._minimizer, self._max_iterations, self._save, self._plot)

AlgorithmFactory.subscribe(FuryFitMultiple)
