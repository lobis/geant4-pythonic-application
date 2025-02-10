from __future__ import annotations

from geant4_python_application._geant4_application import (
    awkward_version,
    geant4_version,
    pybind11_version,
)
from geant4_python_application._version import version, version_tuple
from geant4_python_application.application import Application
from geant4_python_application.detector import Detector
from geant4_python_application.files.datasets import (
    application_data_directory as data_directory,
)
from geant4_python_application.files.datasets import install_datasets
from geant4_python_application.files.directories import application_directory
from geant4_python_application.gdml import basic_gdml

__version__ = version
__version__tuple__ = version_tuple

__all__ = [
    "Application",
    "Detector",
    "__version__",
    "__version__tuple__",
    "application_directory",
    "awkward_version",
    "basic_gdml",
    "data_directory",
    "geant4_version",
    "install_datasets",
    "pybind11_version",
    "version",
]
