# -*- python -*-

import os

# special variables
VERSION = "0.0.3"
top = "."
out = "build"

# subdirectories
testdir   = "test"
subdirs   = " ".join([testdir])

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
  conf.env.set_variant("Release")

  conf.recurse(subdirs)

def build(bld):
  obj = bld.new_task_gen("cxx", "shlib", "node_addon")

  obj.target = "node-icu-charset-detector"
  obj.source = "node-icu-charset-detector.cpp"
  obj.uselib = "ICU"

  bld.recurse(subdirs)
