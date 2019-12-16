from django.http import HttpRequest, HttpResponse
from django.shortcuts import render, redirect
from .web_forms import bees_config_form, token_config_form

def index(request):
    bees_config = open('bees.conf')
    token_config = open('token.base')
    context = {'bees_config': [], 'token_config': []}
    for line in bees_config:
        context['bees_config'].append(['delete_line?file=bees_config&line=' + line])
        line.replace(' ', '')
        mac_address, topic = line.split('=')
        context['bees_config'][-1].insert(0, topic)
        context['bees_config'][-1].insert(0, mac_address)
    for line in token_config:
        context['token_config'].append(['delete_line?file=token_config&line=' + line])
        line.replace(' ', '')
        token, topic, command = line.split(':')
        context['token_config'][-1].insert(0, command)
        context['token_config'][-1].insert(0, topic )
        context['token_config'][-1].insert(0, token)
    return render(request, template_name='index.html', context=context)


def delete_line_in_config(filename, line):
    f = open(filename, 'r')
    lines = []
    for l in f:
        if not l.startswith(line):
            lines.append(l)
        print("'{}'".format(l))
        print("'{}'".format(line))
    f.close()
    f = open(filename,'w')
    for l in lines:
        f.write(l)


def delete_string(request):
    file = request.GET.get('file')
    line = request.GET.get('line')
    if file == 'bees_config':
        delete_line_in_config('bees.conf', line)
    if file == 'token_config':
        delete_line_in_config('token.base', line)
    return redirect('/config_editor')


def append_to_file(filename, line):
    f = open(filename, 'a')
    f.write(line)
    f.write('\n')
    f.close()


def add_string(request):
    print(request.POST)
    file = request.GET.get('file')
    if file == 'bees_config':
        form = bees_config_form(request.POST)
        append_to_file('bees.conf', '{} = {}'.format(form.data['mac_address'], form.data['mqtt_topic']))
    if file == 'token_config':
        form = token_config_form(request.POST)
        append_to_file('token.base', '{} : {} : {}'.format(form.data['token'], form.data['topic'], form.data['command']))
    return redirect('/config_editor')
