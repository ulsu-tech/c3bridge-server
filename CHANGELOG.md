# Changelog

## [1.1.2] (Freeware) - 2021-03-19
Freeware version released together with C3 Easy Control
### Added
  - New command line arguments:
    * -debug — starts the program with the main window displayed and enables the output of debugging messages in the log
    * -verbose — enables the showing of all possible messages in the log
  - New features:
    * CommandFileSetAttribute
    * CommandFileNameList
    * CommandFileCreate
    * CommandFileDelete
    * CommandFileCopy
    * CommandFileMove
    * CommandFileGetProperties
    * CommandFileGetFullName
    * CommandFileGetKrcName
    * CommandFileWriteContent
    * CommandFileReadContent
### Changed
  - Improved logging with a limit on the number of lines to reduce memory consumption
  - New code for generating and processing network messages
### Fixed
  - Fixed bug with uninformative message about client disconnection
### Known issues
  - CommandReadMultiple is implemented incorrectly
  - CommandWriteMultiple is implemented incorrectly

## [1.0.0] (Open Source) - 2020-08-21
The first version of the C3 Bridge Interface Server
### Implemented features
  - CommandReadVariableAscii
  - CommandWriteVariableAscii
  - CommandReadVariable
  - CommandWriteVariable
  - CommandReadMultiple (*BUG*)
  - CommandWriteMultiple (*BUG*)
  - CommandProgramControl
  - CommandMotion
  - CommandKcpAction
  - CommandProxyInfo
  - CommandProxyFeatures
  - CommandCrossConfirmAll
### Known issues
  - Uninformative message about client disconnection
  - CommandReadMultiple is implemented incorrectly
  - CommandWriteMultiple is implemented incorrectly
