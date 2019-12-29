project(
        'wlogout', 
        'c', 
        version: '0.2.1',
        license: 'MIT'
)
gtk = dependency('gtk+-wayland-3.0')
layershell = dependency('gtk-layer-shell-0')
executable('wlogout', 'main.c', dependencies : [gtk, layershell])
