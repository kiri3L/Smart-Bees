# Generated by Django 2.2.6 on 2019-10-07 08:08

from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='gate_uid',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('gate', models.CharField(max_length=128)),
                ('uid', models.CharField(max_length=128, null=True)),
            ],
        ),
    ]