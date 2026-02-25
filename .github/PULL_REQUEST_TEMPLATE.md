## Summary

<!-- One or two sentences describing what this PR does and why. -->

## Related issue(s)

<!-- Link any related issues: "Closes #N" or "Fixes #N" -->

## Changes made

<!-- Bullet-point list of concrete changes. -->
- 
- 

## Flash / SRAM impact

| Metric | Before | After | Delta |
|--------|--------|-------|-------|
| Flash (bytes) | | | |
| Global SRAM (bytes) | | | |

<!-- Run `arduino-cli compile --fqbn arduino:avr:uno .` and paste the last two lines here. -->

## Testing

<!-- How did you verify the change works? -->
- [ ] Compiled successfully with `arduino-cli compile --fqbn arduino:avr:uno .`
- [ ] Flashed to hardware and manually tested affected game(s)
- [ ] No unrelated files changed

## Checklist

- [ ] Code follows the style of the surrounding files (tabs, same variable conventions)
- [ ] New PROGMEM data is read via `pgm_read_byte()` / `pgm_readimg()`
- [ ] No new `volatile` added unless variable is genuinely shared with an ISR
- [ ] `library.properties` `version` bumped if the public API changed
- [ ] Documentation updated if behaviour changed
