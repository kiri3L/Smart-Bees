from django.urls import path
from config_website.views import index, add_string, delete_string

urlpatterns = [
    path('config_editor', index),
    path('delete_line', delete_string),
    path('add_line', add_string)
]