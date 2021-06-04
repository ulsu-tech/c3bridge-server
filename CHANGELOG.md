# Changelog
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
