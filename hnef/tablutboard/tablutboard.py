import jinja2
import os
import webapp2
from google.appengine.api import channel
from google.appengine.api import users
from google.appengine.ext import db

class Ruleset(db.Model):
  name = db.StringProperty()
  # an object holding the rules, encoded in js, to be returned to client
  js = db.StringProperty()
  # this game's variant initial position
  fen_start = db.StringProperty()
  created_on = db.DateTimeProperty(auto_now_add=True)
  created_by = db.UserProperty(auto_current_user_add=True)
    
class Game(db.Model):
  """All the data we store for a game"""
  userX = db.UserProperty()
  userO = db.UserProperty()  
  fen_current = db.StringProperty()
  # We need to store some info about rules sets, so perhaps not just id, but a ref here
  rules = db.ReferenceProperty(Ruleset)
  board = db.StringProperty()
  moveX = db.BooleanProperty()
  winner = db.StringProperty()
  
  game_pgn = db.TextProperty()
  # To track games that were abandoned
  created_on = db.DateTimeProperty(auto_now_add=True)
  # when the first move was made
  started_on = db.DateTimeProperty()
  # last move, or when won/drawn
  last_move_made_on = db.DateTimeProperty(auto_now=True)

class MainPage(webapp2.RequestHandler):
  """This page is responsible for showing the game UI. It may also
  create a new game or add the currently-logged in user to a game."""

  def get(self):
    user = users.get_current_user()
    if not user:
      self.redirect(users.create_login_url(self.request.uri))
      return

    game_key = self.request.get('gamekey')
    game = None
    if not game_key:
      # If no game was specified, create a new game and make this user
      # the 'X' player.
      # With tablutboard, user can play a few games simultaneously, and we also
      # archive games. So here we'll generate a game key using
      # another player has joined, we form 
      game_key = user.user_id()
      game = Game(key_name = game_key,
                  userX = user,
                  moveX = True,
                  board = '         ')
      game.put()
    else:
      game = Game.get_by_key_name(game_key)
      if not game.userO and game.userX != user:
        # If this game has no 'O' player, then make the current user
        # the 'O' player.
        game.userO = user
        game.put()

    token = channel.create_channel(user.user_id() + game_key)
    #game_created = "It is now %s." % game.created_on
    template_values = {'token': token,
                       'me': user.user_id(),
                       'game_key': game_key,
                       'created' : game.created_on
                       }
    template = jinja_environment.get_template('index.html')
    self.response.out.write(template.render(template_values))

jinja_environment = jinja2.Environment(
    loader=jinja2.FileSystemLoader(os.path.dirname(__file__)))
app = webapp2.WSGIApplication([('/', MainPage)],
                              debug=True)
