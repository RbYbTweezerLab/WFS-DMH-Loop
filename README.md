# WFS-DMH-Loop
This is a project to construct a programmable closed-loop made of Thorlabs [DMH40/DMP40](https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=5056) series deformable mirrors and [WFS](https://www.thorlabs.com/newgrouppage9.cfm?objectgroup_id=5287) series wavefront sensors aiming to adjust the beam aberration in the optical setup of our [RbYb Tweezer Lab](https://porto.jqi.umd.edu/).

**If you are a member of the Lab, please access the project's [OneNote](https://onedrive.live.com/view.aspx?resid=601343494F74D454%215914&id=documents&wd=target%28Setting%20up%20Hardware%20Vol.%202.one%7C4494AEAA-32A1-4BE6-AB27-3721055533CD%2FAberration%20control-loop%20with%20deformable%20mirror%20and%20wavefront%20sensor%7C8266EDD0-1606-40D5-849C-D49D2D8196F3%2F%29) page.**

*The project is currently under construction and testing.*

## Author Info:
**Created by:** Juntian "Oliver" Tu  
**E-mail:** [juntian@umd.edu](mailto:juntian@umd.edu)  
**Address:** 2261 Atlantic Building, 4254 Stadium Dr, College Park, MD 20742

## Environment Requirement
The project is developed in `C` and based on the [`TLDFM`, `TLDFMX`](https://www.thorlabs.com/software_pages/ViewSoftwarePage.cfm?Code=DMP40), and [`WFS`](https://www.thorlabs.com/software_pages/ViewSoftwarePage.cfm?Code=WFS) SDK from Thorlabs under 64-bit Windows environment.

The program is designed to work with one deformable mirror and one wavefront sensor connected to the computer through USB. You are able to select between different devices when running the executable.

## Project Structure:
*Under Construction*
The program aims to achieve a close-loop to stabilize the Zernikes coefficients of order $n = 2, 3,$ and $4$ measured by the wavefront sensor to desired values. The code takes measurements from the wavefront sensor and passes the *processed* (see [Current Status](#current-status)) data to the `TLDFMX_get_flat_wavefront()` function, which would calculate the voltages needed by each segment in the deformable mirror to generate a plane wave. `TLDFM_set_segment_voltages()` is then called with the calculated voltages to change the shape of the wavefront.

There exists a separate thread to terminate the loop in case of the divergence of error.

## Current Status

By design of Thorlabs, `TLDFMX_get_flat_wavefront()` only calculates the voltages needed to generate a plane wave (i.e. a zero-Zernikes wavefront), whereas we would like to adjust the Zernikes to a desired value. To achieve this, the current implementation being tested is to subtract the measured Zernikes by the desired Zernikes *before* passing them to `TLDFMX_get_flat_wavefront()`. This method, however, is worried due to the fact that the mathematics process used by `TLDFMX_get_flat_wavefront()` is unclear, and whether it could generate the correct mirror pattern to let the Zernike coefficients converge to non-zero values is unknown.

Moreover, the optical setting in our lab is not the same as the recommended setting documented in the deformable mirror manual. It is proven on the official software that the step of **Control Loop Parameter Determination** (which should correspond to the `TLDFMX_measure_system_parameters()`) could not executed successfully in our optical system. Thus it might be worth trying to bypass the step somehow even though it is documented to be required before engaging the loop.

Once the principle above is proven, a communication channel for remote control by the master computer in the lab is to be constructed.