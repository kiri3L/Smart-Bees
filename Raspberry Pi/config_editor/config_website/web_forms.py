from django import forms


class bees_config_form(forms.Form):
    mac_address = forms.CharField(max_length=11, min_length=11)
    mqtt_topic = forms.CharField(max_length=128)


class token_config_form(forms.Form):
    token = forms.CharField(max_length=128)
    topic = forms.CharField(max_length=128)
    command = forms.CharField(max_length=128)