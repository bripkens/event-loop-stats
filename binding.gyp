{
	"targets": [{
		"target_name" : "eventLoopStats",
		"sources"     : [ "src/eventLoopStats.cc" ],
		"include_dirs" : [
			"src",
			"<!(node -e \"require('nan')\")"
		]
	}]
}
