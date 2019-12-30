project(
        'wlogout', 
        'c', 
        version: '0.3',
        license: 'MIT'
)

datadir = get_option('datadir')
sysconfdir = get_option('sysconfdir')

gtk = dependency('gtk+-wayland-3.0')
layershell = dependency('gtk-layer-shell-0')

install_subdir('assets', install_dir : datadir / 'wlogout')
install_data(['layout', 'style.css'], install_dir : sysconfdir / 'wlogout')

executable('wlogout', 'main.c', dependencies : [gtk, layershell], install : true)
