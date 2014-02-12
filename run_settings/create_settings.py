from configobj import ConfigObj
from sets import Set

def is_correct():
    answer = raw_input('correct? [Y/n]')
    print ''
    if answer == '' or answer == 'y' or answer == 'Y':
        
        return True
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
                if answer[0] < answer[1]:
                    print question,answer
                    return answer
            except:
                pass
    print 'Cannot read FidCut),Please Try again '
    get_fidCut(question)

        

def get_setting_changes(diaName):
    changes = {}
    print 'Settings for diamond "%s"\n'%diaName

    firstChannel = get_int('Get First connected channel: ')
    changes['firstChannel'] = firstChannel
    changes['lastChannel'] = get_int( 'Get Last  connected channel: ' )
    changes['notConnectedChannels'] = get_set('get not connected Channels: ')
    changes['noisyChannels'] = get_set('get noisy channels: ')
    changes['maskedChannels'] = get_set('get masked channels')
    changes['fidCutX'] = get_fidCut('Get FidCutX (e.g. "12 - 55"): ')
    changes['fidCutY'] = get_fidCut('Get FidCutY (e.g. "12 - 55"): ')
    print changes
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
        config.pop(key)


def set_masked_channels(config,changes):
    noisyChannels = Set()
    notConnectedChannels = Set()
    maskedChannels = Set()
    for i in changes:
        notConnectedChannels.union_update(changes[i]['notConnectedChannels'])
        noisyChannels.union_update(changes[i]['noisyChannels'])
        maskedChannels.union_update(changes[i]['maskedChannels'])
    print 'maskedChannels: %s'%maskedChannels
    print 'noisyChannels:  %s'%noisyChannels
    print 'notConnected:   %s'%notConnectedChannels
    config['Dia_channel_screen_channels'] = '{'+', '.join(['%s'%i for i in list(maskedChannels)])+'}'
    config['Dia_channel_noisy_channels'] = '{'+', '.join(['%s'%i for i in list(noisyChannels)])+'}'
    config['Dia_channel_not_connected'] = '{'+', '.join(['%s'%i for i in list(notConnectedChannels)])+'}'


def set_settings(config,changes):
    print 'set settings :', changes
    for i in changes:
        print i,changes[i]
        if 'FidCutX' in i:
            config['si_avg_fidcut_xlow']=changes[i][0]
            config['si_avg_fidcut_xhigh']=changes[i][1]
        elif 'FidCutX' in i:
            config['si_avg_fidcut_ylow']=changes[i][0]
            config['si_avg_fidcut_yhigh']=changes[i][1]
            
        elif 'Channels' in i:
            continue
            if type(changes[i]) == Set:
                changes[i] = list(changes[i])
            strings = ['%s'%d for d in changes[i]]
            strings = ', '.join(strings)
            print i,changes[i],strings
        config[i] = changes[i]
    




config = ConfigObj('example.ini',raise_errors=True,list_values=False,)

for i in config:
    print '%s: %s,%s'%(i,config[i],type(config[i]))

#raw_input('Which run? ',runnumber)
while True:
    runNo = get_int('What run no?')
    events = get_int('How many Events?')
    voltage =get_int("What voltage?")
    repeaterCard = get_int("Which repeater card no?")
    currentBegin = get_float('Current Begin? ')
    currentEnd   = get_float('Current End?')
    nDiamonds = get_int('How many diamonds?')
            
    print ''
    print 'RunNo:          %5s'%runNo
    print 'Events:         %7s'%events
    print 'Voltage:        %5s V'%voltage
    print 'RepeaterCard:   %5s'%repeaterCard
    print 'current Begin:  %5f'%currentBegin
    print 'current End:    %5f'%currentEnd
    print 'No of Diamonds: %5s'%nDiamonds
    if is_correct():
        break


while True:
    diaNames = ['']*nDiamonds
    for i in range(0,nDiamonds):
        diaNames[i] = raw_input('Name of diamond %s: '%i)

    print ''
    print diaNames
    if is_correct():
        break
changes ={}
for i in range(0,nDiamonds):
    print 'Define Settings for diamond no %s'%(i+1)
    changes[diaNames[i]] = get_setting_changes(diaNames[i])

for i in changes:
    print i, changes[i]


config['runNo'] = '%s'%runNo
config['Events'] = '%s'%events
config['voltage'] = '%s'%voltage
config['repeaterCardNo'] = '%s'%repeaterCard
config['currentBegin'] = '%s'%currentBegin
config['currentEnd'] = '%s'%currentEnd
config['nDiamonds'] = nDiamonds

set_masked_channels(config,changes)
set_patterns(config,changes,diaNames)

for i in range(0,nDiamonds):
    mainName = 'test'
    if nDiamonds == 1:
        config.filename = '%s.%d.ini'%(mainName,runNo)
    elif i == 0:
        config.filename = '%s.%d-left.ini'%(mainName,runNo)
    elif i == 1:
        config.filename = '%s.%d-right.ini'%(mainName,runNo)
    config['diamondName']='%s'%diaNames[i]
    set_settings(config,changes[diaNames[i]])

config.write()
