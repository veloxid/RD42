from configobj import ConfigObj
import os
from sets import Set

def is_correct():
    answer = raw_input('correct? [Y/n]')
    print ''
    if answer == '' or answer == 'y' or answer == 'Y':
        
        return True
    return False

def get_yes_no_answer(question,default=False):
    addition = '\t'
    if default==False: addition+='[y/N]'
    else: addition +='[Y/n]'
    while True:
        answer = raw_input(question+addition)
        answer = answer.lower()
        if answer == '':
            return default
        elif  answer.startswith('y'):
            return True
        elif answer.startswith('n'):
            return False

def get_int(question):
    while True:
        try:
            retVal = int(raw_input(question+'\t'))
            return retVal
        except:
            pass

def get_float(question):
    while True:
        try:
            retVal = float(raw_input(question+'\t'))
            return retVal
        except:
            pass
def get_list(question):
    pattern = raw_input(question +'\t')
    pattern = pattern.strip()
    if not pattern.startswith('{'):
        pattern = '{'+pattern
    if not pattern.endswith('}'):
        pattern+= '}'
    print pattern
    return pattern

def convert_to_set(i):
    channels = Set()
    if '-' in i:
         i = i.split('-')
         if len(i) != 2:
             return channels
         try:
             i = [int(k) for k in i]
             for j in range(i[0],i[1]):
                 channels.add(j)
         except:
             pass
         return channels
    else:
        try:
            i = int(i)
            channels.add(i)
            pass
        except:
            pass
    return channels

def get_set(question):
    answer = raw_input(question+'\t')
    answer = answer.replace(' ',',').split(',')
    channels = Set()
    for i in answer:
        channelSet = convert_to_set(i)
        channels.union_update(channelSet)
    return channels


def get_fidCut(question):
    answer = raw_input(question)
    if '-' in answer:
        answer = answer.split('-')
        if len(answer) == 2:
            try:
                answer = [float(i) for i in answer]
                if answer[0] < answer[1] and answer[0]>= 0 and answer[1]<=256:
                    print question,answer
                    return answer
            except:
                pass
    print 'Cannot read fidCut),Please Try again '
    return get_fidCut(question)

        

def get_setting_changes(diaName,allChanges,retry):
    if allChanges.has_key(diaName):
        changes = allChanges[diaName]
        print 'found "old" changes',changes
    else:
        print 'No Old Changes for %s'%diaName
        changes = {}
    nChannels = 128
    print 'Settings for diamond "%s"'%diaName
    

    while True:
        firstChannel = None
        if not retry:
            firstChannel = get_int('Get First connected channel: ')
            while not firstChannel in range(0,nChannels-1):
                print 'first channel must be in range  0 - %d'%(nChannels-1)
                firstChannel = get_int('Get First connected channel: ')
            changes['firstChannel'] = firstChannel
            lastChannel  = get_int( 'Get Last  connected channel: ' )
            while not lastChannel in range(firstChannel+1,nChannels):
                lastChannel = get_int( 'Get Last  connected channel: ' )
            changes['lastChannel'] = lastChannel

        if not retry or get_yes_no_answer('Do you want to change masked channels?'):
            changes['notConnectedChannels'] = get_set('get not connected Channels: ')
            changes['noisyChannels'] = get_set('get noisy channels: ')
            changes['maskedChannels'] = get_set('get masked channels')
        if firstChannel != None:
            changes['maskedChannels'].update([firstChannel,firstChannel+1,lastChannel-1,lastChannel])

        print 'Masked Channels: \t%s'%changes['maskedChannels']
            
        changes['fidCutX'] = get_fidCut('Get fidCutX (e.g. "12 - 55"): ')
        changes['fidCutY'] = get_fidCut('Get fidCutY (e.g. "12 - 55"): ')
        while changes['fidCutY'] == None:
            changes['fidCutY'] = get_fidCut('Get fidCutY (e.g. "12 - 55"): ')

        changes['Comment'] = raw_input('Please enter a comment:\t')
        print changes
        if get_yes_no_answer('Are this changes correct?',True):
            return changes

def removekey(d, key):
        r = dict(d)
        del r[key]
        return r

def set_patterns(config,changes,diaList):
    i = 0
    pos = 0

    for dia in diaList:
        if not changes.has_key(dia):
            continue
        if not changes[dia].has_key('firstChanel'):
            continue
        if not changes[dia].has_key('lastChanel'):
            continue
        firstChannel = changes[dia]['firstChannel']
        lastChannel  = changes[dia]['lastChannel']
        pitchWidth = 50 #mum

        #diamondPattern = {0,50,6,61};
        diamondPattern =  '{%d,%d,%d,%d}'%(pos,pitchWidth,firstChannel,lastChannel)
        key = 'diamondPattern%d'%i
        changes[dia][key] = diamondPattern
        config[key] = diamondPattern
        changes[dia] =  removekey(changes[dia],'firstChannel')
        changes[dia] =  removekey(changes[dia],'lastChannel')
        pos = lastChannel*pitchWidth+300 
        pos = round(pos/500+.5)*500
        i +=1
    for i in range(len(diaList),3):
        key = 'diamondPattern%d'%i
        if config.has_key(key):
            config.pop(key)


def set_masked_channels(config,changes):
    noisyChannels = Set()
    notConnectedChannels = Set()
    maskedChannels = Set()
    for i in changes:
        notConnectedChannels.update(changes[i]['notConnectedChannels'])
        noisyChannels.update(changes[i]['noisyChannels'])
        maskedChannels.update(changes[i]['maskedChannels'])
    for j in noisyChannels:
        maskedChannels.update([j-1,j+1])
    for j in notConnectedChannels:
        maskedChannels.update([j-1,j+1])
    print 'maskedChannels: %s'%maskedChannels
    print 'noisyChannels:  %s'%noisyChannels
    print 'notConnected:   %s'%notConnectedChannels
    config['Dia_channel_screen_channels'] = '{'+', '.join(['%s'%i for i in list(maskedChannels)])+'}'
    config['Dia_channel_noisy_channels'] = '{'+', '.join(['%s'%i for i in list(noisyChannels)])+'}'
    config['Dia_channel_not_connected'] = '{'+', '.join(['%s'%i for i in list(notConnectedChannels)])+'}'

def set_selection_fidCuts(config,changes,diaNames):
    i = 0
    for dia in diaNames:
        if not changes.has_key(dia):
            continue
        key = 'selectionfidCut%d'%i
        print changes[dia]

        value = '{%03d-%03d,%03d-%03d}'%(changes[dia]['fidCutX'][0],changes[dia]['fidCutX'][1],changes[dia]['fidCutY'][0],changes[dia]['fidCutY'][1])
        #selectionfidCut={065-091,60-104};
        changes[dia][key] = value
        config[key] = value
        i+=1
        config.scalars.pop(config.scalars.index(key))
        keyPos = 'si_avg_fidcut_yhigh'
        config.scalars.insert(config.scalars.index(keyPos)+i,key)


def set_settings(config,changes):
    #print 'set settings :', changes
    for i in changes:
        #print i,changes[i]
        if 'fidCutX' in i:
            config['si_avg_fidcut_xlow']=changes[i][0]
            config['si_avg_fidcut_xhigh']=changes[i][1]
        elif 'fidCutY' in i:
            config['si_avg_fidcut_ylow']=changes[i][0]
            config['si_avg_fidcut_yhigh']=changes[i][1]
            
        elif 'Channels' in i:
            continue
            if type(changes[i]) == Set:
                changes[i] = list(changes[i])
            strings = ['%s'%d for d in changes[i]]
            strings = ', '.join(strings)
            #print i,changes[i],strings
        else:
            config[i] = changes[i]
    

def get_diamond_names(nDiamonds,diaLog):
    diaNames = ['']*nDiamonds
    while True:
        for i in range(0,nDiamonds):
            diaNames[i] = raw_input('Name of diamond %s: '%i)

        print ''
        print diaNames
        if is_correct():
            return diaNames
            break

def get_diamond_log():
    diamondLog = {}
    f = open('runDetails.log')
    for line in f.readlines():
        line = line.strip('\n').split(';')
        line[1] = line[1].strip('[]').replace('"','').replace("'","").split(', ')
        diamondLog[int(line[0])] = {
                'dia': line[1], 
                'voltage': line[2].strip('V'),
                'repeaterCard': line[3].strip('# '),
                'currentBegin': line[4].strip(),
                'currentEnd': line[5].strip()
                }

    for i in sorted(diamondLog.keys()):
        print i,diamondLog[i]
    return diamondLog


config = ConfigObj('example.ini',raise_errors=True,list_values=False,)

for i in config:
    print '%s: %s,%s'%(i,config[i],type(config[i]))

#raw_input('Which run? ',runnumber)
diamondLog = get_diamond_log()
invalid = False
diaNames = []
retry = False
i = 0
changes ={}
while retry or i ==0:
    i+=1
    while True:
        runNo = get_int('What run no?')
        events = get_int('How many Events?')
        if diamondLog.has_key(runNo) and not invalid:
            try:
                voltage = int(diamondLog[runNo]['voltage'])
            except:
                print 'cannot convert "%s"'%diamondLog[runNo]['voltage']
                voltage =get_int("What voltage?")
        else:
            voltage =get_int("What voltage?")

        if diamondLog.has_key(runNo) and not invalid:
            try:
                repeaterCard = diamondLog[runNo]['repeaterCard']
                if '?' in repeaterCard:
                    repeaterCard = -1
                else:
                    repeaterCard = int(repeaterCard)
            except:
                print 'cannot convert "%s"'%diamondLog[runNo]['repeaterCard']
                repeaterCard = get_int("Which repeater card no?")
        else:
            repeaterCard = get_int("Which repeater card no?")

        if diamondLog.has_key(runNo) and not invalid:
            try:
                currentBegin = diamondLog[runNo]['currentBegin']
                if currentBegin == '' or '?' in currentBegin:
                    currentBegin = -1
                elif '<' in currentBegin:
                    currentBegin = float(currentBegin.strip('< '))
                else:
                    currentBegin = float(currentBegin)
            except:
                print 'cannot convert "%s"'%diamondLog[runNo]['currentBegin']
                currentBegin = get_float('Current Begin? ')
        else:
            currentBegin = get_float('Current Begin? ')

        if diamondLog.has_key(runNo) and not invalid:
            try:
                currentEnd = diamondLog[runNo]['currentEnd']
                if currentEnd =='' or '?' in currentEnd:
                    currentEnd = -1
                elif '<' in currentEnd:
                    currentEnd = float(currentEnd.strip('< '))
                else:
                    currentEnd = float(currentEnd)
            except:
                print 'cannot convert "%s"'%diamondLog[runNo]['currentEnd']
                currentEnd   = get_float('Current End?')
        else:
            currentEnd   = get_float('Current End?')

        if diamondLog.has_key(runNo) and not invalid:
            try:
                nDiamonds = len(diamondLog[runNo]['dia'])
                diaNames = diamondLog[runNo]['dia']
            except:
                print 'cannot convert: ',diamondLog[runNo]['dia']
                nDiamonds = get_int('How many diamonds?')
        else:
            nDiamonds = get_int('How many diamonds?')
                
        print ''
        print 'RunNo:          %7s'%runNo
        print 'Events:         %7s'%events
        print 'Voltage:        %5s V'%voltage
        print 'RepeaterCard:   %7s'%repeaterCard
        print 'current Begin:  %7.1f'%currentBegin
        print 'current End:    %7.1f'%currentEnd
        print 'No of Diamonds: %7s'%nDiamonds
        print 'Names of Diamonds: %s'%diaNames
            
        if is_correct():
            break
        else:
            invalid = True

    if invalid or len(diaNames)==0:
        diaNames = get_diamond_names(nDiamonds,diamondLog)

    for i in range(0,nDiamonds):
        print '\nDefine Settings for diamond no %s'%(i+1)
        changes[diaNames[i]] = get_setting_changes(diaNames[i],changes,retry)

    config['runNo'] = '%s'%runNo
    config['Events'] = '%s'%events
    config['voltage'] = '%s'%voltage
    config['repeaterCardNo'] = '%s'%repeaterCard
    config['currentBegin'] = '%s'%currentBegin
    config['currentEnd'] = '%s'%currentEnd
    config['nDiamonds'] = nDiamonds

    set_masked_channels(config,changes)
    set_patterns(config,changes,diaNames)
    set_selection_fidCuts(config,changes,diaNames)

    for i in range(0,nDiamonds):
        mainName = 'settings'
        if nDiamonds == 1:
            config.filename = '%s.%d.ini'%(mainName,runNo)
            corFileName = '%s.%d0.ini'%(mainName,runNo)
        elif i == 0:
            config.filename = '%s.%d-left.ini'%(mainName,runNo)
            corFileName = '%s.%d1-left.ini'%(mainName,runNo)
        elif i == 1:
            config.filename = '%s.%d-right.ini'%(mainName,runNo)
            corFileName = '%s.%d2-right.ini'%(mainName,runNo)
        config['diamondName']='%s'%diaNames[i]
        set_settings(config,changes[diaNames[i]])
        writeFile = True
        if os.path.isfile(config.filename):
            writeFile = get_yes_no_answer('Do you want to overwrite "%s"?'%config.filename)
        if  writeFile:
            print 'write File %s'%config.filename
            config.write()
        if not os.path.islink(corFileName) and os.path.exists(corFileName):
            print 'remove file %s.'%corFileName
            os.remove(corFileName)
        if not os.path.islink(corFileName):
            print 'create link for feed through corrected run'
            os.symlink(config.filename,corFileName)
    retry = get_yes_no_answer('Do you want to enter another run of same testbeam and same diamonds?',True)
    if not retry:
        exit()

    
