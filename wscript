# -*- python -*-

import os

# special variables
VERSION = "0.0.1"
top = "."
out = "build"

# subdirectories
sourcedir = "lib"
testdir   = "test"
subdirs   = " ".join([sourcedir, testdir])

def get_command_output(cmd):
  return os.popen(cmd).read().strip()

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool("compiler_cxx")
  conf.check_tool("node_addon")

  if get_command_output("icu-config --version")[0] != "4":
    conf.fatal("Missing library icu 4.x.x")

  conf.env.append_value("CXXFLAGS_ICU", get_command_output("icu-config --cppflags").split(" "))
  conf.env.append_value("LINKFLAGS_ICU", get_command_output("icu-config --ldflags").split(" "))

  conf.env.set_variant("default")

  conf.recurse(subdirs)

def build(bld):
  bld.recurse(subdirs)
