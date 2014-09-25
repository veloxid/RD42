import os, sys
import helper
from plotter import plotter
import tables
import runlog


class results(object) :

	def __init__(self, config_file, path, output_path, runlog_file) :
		self.config_file = config_file
		if not path.endswith('/') : path += '/'
		self.path = path
		if not output_path.endswith('/') : output_path += '/'
		self.output_path = output_path
		helper.mkdir(self.output_path)
		self.runlog = runlog.runlog(runlog_file)


	def get_results(self, runs) :
		'''get plots and results for a list of runs'''

		plots = ['FidCut', 'PulseHeight', 'Noise']
		results = {}
		for run in runs :
			results[run['number']] = {}
			for plot in plots :
				pl = plotter(self.config_file, self.path, self.output_path, run['number'], run['position'], plot)
				results[run['number']][plot] = pl.plot()
				results[run['number']]['Voltage'] = self.runlog.get_voltage(run['number'])
		tables.make_NoisePulseHeightTable(self.output_path, results)


if __name__ == '__main__' :
	args = sys.argv
	path = './'
	output_path = 'results/'
	runlog_file = '../OverviewPageCreation/config/all_log.txt'
	runs = [{'number': 17100, 'position': ''}]

	if ('--help' in args) or ('-h' in args) :
		print 'usage: ..'
		sys.exit(1)

	if ('-i' in args) :
		path = args[args.index('-i')+1]

	if ('-c' in args) and (args.index('-c')+1 < len(args)) and (not args[args.index('-c')+1].startswith('-')) :
		config_file = args[args.index('-c')+1]
	else :
		config_file = '%s/config.cfg' % os.path.dirname(os.path.realpath(__file__))

	if ('-o' in args) :
		output_path = args[args.index('-o')+1]

	if ('-r' in args) :
		runlog_file = args[args.index('-r')+1]

	if ('--runs' in args) :
		runs_file = args[args.index('--runs')+1]
		if not os.path.isfile(runs_file) :
			print '[ERROR] %s does not exist' % runs_file
			sys.exit(1)
		with open(runs_file, 'r') as file :
			runs = []
			for line in file.readlines() :
				if line[0] == '#' : continue
				splitline = line.split()
				number = splitline[0]
				if len(splitline) < 2 :
					position = ''
				else :
					position = splitline[1]
				run = {}
				run['number']   = int(number)
				run['position'] = position
				runs.append(run)

	res = results(config_file, path, output_path, runlog_file)
	res.get_results(runs)
