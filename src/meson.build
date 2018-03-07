usertool_sources = ['bsd2linux.cc',
                    'daemon.cc',
                    'database.cc',
                    'filtersystem.cc',
                    'flawsearch.cc',
                    'group.cc',
                    'linux2bsd.cc',
                    'passwordupdate.cc',
                    'removeusers.cc',
                    'string_methods.cc',
                    'system.cc',
                    'system2yaml.cc',
                    'systemupdate.cc',
                    'updatedatabase.cc',
                    'user.cc',
                    'usertool.cc']

usertool = executable('usertool', usertool_sources, include_directories : incdir, dependencies: yaml_dep, install: true)
