## GMAT Link
These classes are used to link GMAT to the rest of the code. They are not meant to be used by the user, but rather to be used by the visualization engine with the user input.

### Ejemplo de Configuración

`GMATScripter::createScript()` now expects all GMAT script parameters to exist in the internal config map.

You can load a complete test configuration optionally with:

- `loadConfigFromFile("default_test")`

This reads from `data/default_test_GMAT`.

Required keys include orbital, spacecraft, force model, propagator, subscriber, and simulation fields used in the generated script.