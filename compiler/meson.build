project('Wuvy compiler', 'cpp', default_options : ['c_std=c11', 'cpp_std=c++17'])

# Think about https://mesonbuild.com/Build-options.html and googletest in it

cmake = import('cmake')

settings = {
	'source_dir': 'src',
	'inc_dir': 'include',
	'deplist': [
		cmake.subproject('fmt').dependency('fmt'),
		subproject('gtest').get_variable('gtest_dep')
	]
}

srclist = [
	'@0@/Main.cpp',
	'@0@/Lexer.cpp'
]

sources = []
foreach sf : srclist
	sources += sf.format(settings.get('source_dir', 'src'))
endforeach

executable('wuvyc', sources, dependencies : settings.get('deplist', []), include_directories : settings.get('inc_dir', 'include'))
