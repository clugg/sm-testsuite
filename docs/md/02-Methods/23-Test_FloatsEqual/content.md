---
Title: Test_FloatsEqual
---

Helper function to check whether two floats are "equal" within a threshold (to account for floating point errors).

For more information, see [https://en.wikipedia.org/wiki/Floating_point_error_mitigation](#content-https://en-methods-wikipedia.org/wiki/floating-point-error-mitigation).

#- Arguments
- expected `float`
- First value to compare.
- actual `float`
- Second value to compare.
- threshold `float`
- Maximum allowed difference between floats. *default:* `0.0001`

#- Returns
- `bool`
- 
