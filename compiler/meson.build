project('Wuvy compiler', 'cpp', default_options : ['c_std=c11', 'cpp_std=c++17'])


cmake = import('cmake')

settings = {
	'source_dir': 'src',
	'inc_dir': 'include',
	'deplist': [
		cmake.subproject('fmt').dependency('fmt')
	]
}

srclist = [
	'@0@/main.cpp'
]

sources = []
foreach sf : srclist
	sources += sf.format(settings.get('source_dir', 'src'))
endforeach

executable('wuvyc', sources, dependencies : settings.get('deplist', []), include_directories : settings.get('inc_dir', 'include'))
