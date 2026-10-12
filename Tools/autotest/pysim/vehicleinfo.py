# flake8: noqa

class VehicleInfo(object):

    def __init__(self):
        """
        waf_target: option passed to waf's --target to create binary
        default_params_filename: filename of default parameters file.  Taken to be relative to autotest dir.
        extra_mavlink_cmds: extra parameters that will be passed to mavproxy
        """
        self.options = {
    "ArduPlane": {
        "default_frame": "plane",
        "frames": {
            # PLANE
            "quadplane-tilttri": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm",
                                            "default_params/quadplane-tilttri.parm"],
            },
            "quadplane-tilttrivec": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm",
                                            "default_params/quadplane-tilttrivec.parm"],
            },
            "quadplane-tilthvec": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/quadplane-tilthvec.parm"],
            },
            "quadplane-tri": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm",
                                            "default_params/quadplane-tri.parm"],
            },
            "quadplane-cl84" : {
                "waf_target" : "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm",
                                            "default_params/quadplane-cl84.parm"],
            },
            "quadplane": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "default_params/quadplane.parm",
            },
            "quadplane-ice": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm", "default_params/plane-ice.parm", "default_params/quadplane-ice.parm"],
            },
            "quadplane-can": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm", "default_params/quadplane-can.parm"],
                "periph_params_filename": ["default_params/periph.parm", "default_params/quadplane-periph.parm"],
            },
            "quadplane-tilt": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm",
                                            "default_params/quadplane-tilt.parm"],
            },
            "firefly": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm",
                                            "default_params/firefly.parm"]
            },
            "plane-elevon": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-elevons.parm"],
            },
            "plane-vtail": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-vtail.parm"],
            },
            "plane-tailsitter": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "default_params/plane-tailsitter.parm",
            },
            "plane-jet": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-jet.parm"],
            },
            "plane-ice": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-ice.parm"],
            },
            "plane-3d": {
                "waf_target": "bin/arduplane",
                "default_params_filename": [], # defaults are loaded in SIM_Plane.cpp
            },
            "glider": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "default_params/glider.parm",
            },
            "quadplane-copter_tailsitter": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["default_params/quadplane.parm","default_params/quadplane-copter_tailsitter.parm"],
            },
            "plane": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "models/plane.parm",
            },
            "plane-dspoilers": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-dspoilers.parm"]
            },
            "plane-redundant": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-redundant.parm"]
            },
            "plane-soaring": {
                "waf_target": "bin/arduplane",
                "default_params_filename": ["models/plane.parm", "default_params/plane-soaring.parm"]
            },
            "gazebo-zephyr": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "default_params/gazebo-zephyr.parm",
                "external": True,
            },
            "last_letter": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "models/plane.parm",
                "external": True,
            },
            "CRRCSim": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "models/plane.parm",
                "external": True,
            },
            "jsbsim": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "default_params/plane-jsbsim.parm",
                "external": True,
            },
            "scrimmage-plane" : {
                "waf_target": "bin/arduplane",
                "default_params_filename": "models/plane.parm",
                "external": True,
            },
            "calibration": {
                "extra_mavlink_cmds": "module load sitl_calibration;",
                "external": True,  # lies!  OTOH, hard to take off with this
            },
            "stratoblimp": {
                "waf_target": "bin/arduplane",
                "default_params_filename": "default_params/stratoblimp.parm",
            },
        },
    },
    "sitl_periph_universal": {
        "frames": {
            "universal": {
                "configure_target": "sitl_periph_universal",
                "waf_target": "bin/AP_Periph",
                "default_params_filename": "default_params/periph.parm",
                },
            }
    },
}


    def default_frame(self, vehicle):
        return self.options[vehicle]["default_frame"]

    def default_waf_target(self, vehicle):
        """Returns a waf target based on vehicle type, which is often determined by which directory the user is in"""
        default_frame = self.default_frame(vehicle)
        return self.options[vehicle]["frames"][default_frame]["waf_target"]

    def options_for_frame(self, frame, vehicle, opts):
        """Return information about how to sitl for frame e.g. build-type==sitl"""
        ret = None
        frames = self.options[vehicle]["frames"]
        if frame in frames:
            ret = self.options[vehicle]["frames"][frame]
        else:
            for p in ["firefly", "gazebo", "last_letter", "jsbsim", "quadplane", "plane-elevon", "plane-vtail", "plane"]:
                if frame.startswith(p):
                    ret = self.options[vehicle]["frames"][p]
                    break
        if ret is None:
            print("WARNING: no config for frame (%s)" % frame)
            ret = {}

        if "model" not in ret:
            ret["model"] = frame

        if "sitl-port" not in ret:
            ret["sitl-port"] = True

        if opts.model is not None:
            ret["model"] = opts.model

        if (ret["model"].find("xplane") != -1 or ret["model"].find("flightaxis") != -1):
            ret["sitl-port"] = False


        if "waf_target" not in ret:
            ret["waf_target"] = self.default_waf_target(vehicle)

        if opts.build_target is not None:
            ret["waf_target"] = opts.build_target

        return ret



