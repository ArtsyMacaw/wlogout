project(
        'wlogout', 
        'c', 
        version: '1.1',
        license: 'MIT',
        default_options:
        [
            'c_std=c11',
            'werror=true'
        ]
)

add_project_arguments(
        [
            '-Wno-unused-parameter',
		    '-Wno-unused-result',
		    '-Wno-missing-braces'
        ],
        language: 'c'
)

datadir = get_option('datadir')
sysconfdir = get_option('sysconfdir')

scdoc = dependency('scdoc', required: get_option('man-pages'))
if scdoc.found()
	scdoc_prog = find_program(scdoc.get_pkgconfig_variable('scdoc'), native: true)
	sh = find_program('sh', native: true)
	mandir = get_option('mandir')
	man_files = [
			'man/wlogout.1.scd',
			'man/wlogout.5.scd'
	]
	foreach filename : man_files
		topic = filename.split('.')[-3].split('/')[-1]
		section = filename.split('.')[-2]
		output = '@0@.@1@'.format(topic, section)

		custom_target(
			output,
			input: filename,
			output: output,
			command: [
				sh, '-c', '@0@ < @INPUT@ > @1@'.format(scdoc_prog.path(), output)
			],
			install: true,
			install_dir: '@0@/man@1@'.format(mandir, section)
		)
	endforeach
endif

if get_option('bash-completions')
	bashdir = datadir + '/bash-completion/completions'
	install_data('completions/wlogout.bash', install_dir: bashdir)
endif

if get_option('fish-completions')
	fishdir = datadir + '/fish/completions'
	install_data('completions/wlogout.fish', install_dir: fishdir)
endif

if get_option('zsh-completions')
	zshdir = datadir + '/zsh/site-functions'
	install_data('completions/_wlogout', install_dir: zshdir)
endif

gtk = dependency('gtk+-wayland-3.0')
layershell = dependency('gtk-layer-shell-0', required : false)

if layershell.found()
    add_project_arguments('-DLAYERSHELL=1', language : 'c')
endif

install_subdir('assets', install_dir : datadir / 'wlogout')
install_subdir('icons', install_dir : datadir / 'wlogout')
install_data(['layout', 'style.css'], install_dir : sysconfdir / 'wlogout')

executable('wlogout', 'main.c', dependencies : [gtk, layershell], install : true)
