# PROWOGENE 2.0

The main changes are aimed to improve architecture. The list of changes:

* `Storage` class was deleted. Now all data must be linked explicitly. The previous implementation can lead to crashes when types are mixed by an accident. Also it was too complicated way to organize data.
* Standard module's data pointers were changed from `protected` to `public` for simpler data linkage without storage.
* `Logger` class was separated to manager class (`Logger`) and classes specific to only one output (like `StdoutLogWriter`) which are managed by `ILogWriter` interface. Now it's much simpler to add new functionality to `Logger`.
* Generator now uses "dummy" logger by default and no null pointer checks are needed before `Logger` calls. Also fixed a crash when no `Logger` are passed to the `Generator` (or `nullptr` is passed).
* Exceptions are used in modules instead of `true`/`false` retcodes. It's much easier and informative now, isn't it?
* Generator checks only needed settings (not just all inside config file like it was). Also every setting will be checked only once during the pipeline.
* Other minor improovements to simplify code.
* Added simple scripts for simple package creation.


# PROWOGENE 1.0

Library released!
