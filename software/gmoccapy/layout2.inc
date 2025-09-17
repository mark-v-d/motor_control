[XHC_HB04_BUTTONS]
# use button names according to layout file LIB:xhc-hb04-layout{n}.cfg
# note: "start-pause" is connected for standard behavior
#        controlling halui.pause/halui.resume/halui.run
# these are examples, edit as required:

#
# Yellow button, create waypoint
#
goto-zero   = ""
# synthesized per-axis buttons for goto-zero button:
goto-zero-x = halui.mdi-command-00
goto-zero-y = ""
goto-zero-z = halui.mdi-command-04
goto-zero-a = ""

#start-pause = std_start_pause
rewind      = halui.program.step
# probe-input for simulating a probe:
probe-z     = ""
macro-3     = halui.mdi-command-03

half        = halui.spindle.stop
# synthesized per-axis buttons for half button:
half-x      = ""
half-y      = ""
half-z      = ""
half-a      = ""

zero        = ""
# synthesized per-axis buttons for zero button:
zero-x      = halui.mdi-command-12
zero-y      = halui.mdi-command-13
#zero-z      = halui.mdi-command-14
zero-a      = halui.mdi-command-15

safe-z      = halui.mdi-command-10
# in this sim, home-all only works first time, hold down till finished:
home        = halui.home-all
#macro-1     = halui.mdi-command-01
macro-2     = halui.mdi-command-02
spindle     = halui.spindle.start
step        = xhc-hb04.stepsize-up
mode        = ""
macro-6     = halui.mdi-command-06
macro-7     = halui.mdi-command-07
#stop        = halui.program.stop
#reset       = halui.estop.activate
