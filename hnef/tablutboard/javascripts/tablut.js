// If Firebug is not installed, prevent console.log() from creating error messages
if (typeof console == "undefined") { var console = { log: function() {} }; }

// Iterate within an arbitrary context...
jQuery.eachWithContext = function(context, object, callback) {
  for ( var i = 0, length = object.length, value = object[0];
    i < length && callback.call(context, i, value ) !== false; value = object[++i] ) {}
};

(function($) {
  /* Constructor */
  $.tablut = function(options, wrapper) {
    this.settings = $.extend( {}, $.tablut.defaults, options );
    this.wrapper = wrapper;

    this.game = {

      move_origin : { top : 0, left : 0 },
      halfmove_clock : 0,

      transition_current : 0,

      header : [],
      body : '',
      moves : [],
      annotations : [],
      raw_annotations : [],

      next_piece_id : 64,
      transition : {},
      transitions : [],
      board_direction : 1,
      board_size : 9
    };

  };

/* Add tablut() to the jQuery namespace */
  $.fn.tablut = function(options) {
    var tablut = new $.tablut(options, this[0]);
    tablut.init();
    return tablut;
  };

  $.extend($.tablut, {

    defaults : {
      fen : "3aaa3/4a4/4p4/a3p3a/aappkppaa/a3p3a/4p4/4a4/3aaa3",
      square_size : 70,
      offsets : { left: 0, top: 0 },
      board_element_selector : '.tablut-board',
      json_annotations : false,
      board_size : 9,
      rules : {
        first_move : 'b',
        king_in_captures : { 'hammer' : false, 'anvil' : false },
        citadels : { captures : { 'corner' : true, 'throne' : false },
                     confinement : { 'throne' : true, 'corner' : false } },
        soldiers_throne : { 'enter' : false, 'pass' : false }
      }
    },

    prototype : {
      init : function() {
        // Load a fresh board position
        this.setUpBoard( this.parseFEN( this.settings.fen ) );

        // If pgn was passed in, parse it
        //if (this.settings.pgn) this.parsePGN(this.settings.pgn);

        //this.setUpBoard( this.parseFEN( this.settings.fen ) );
        this.writeBoard();
        //this.game.active_color = this.settings.rules.first_move;
        this.showActiveSide();
      },

      transitionBackward : function() {
        if (this.game.transition_current == 0)
		return;
	this.game.transition_current = this.game.transition_current - 1;
	var transToRollBack = this.game.transitions[this.game.transition_current];
	// un-fade pieces still present on the board
	if (transToRollBack.winner) {
        $.eachWithContext(this, this.boardData(), function(j, row) {
          $.eachWithContext(this, row, function(k, piece) {
            if (piece != '-') {
               var node = $('#' + this.getDomPieceId(piece.id));
               node.show('fade');
            }
          });
        });
        }

	if (transToRollBack.captured_pieces)
	// restore all captured pieces
        $.eachWithContext(this, transToRollBack.captured_pieces, function(j, capture) {
          var node = $('#' + this.getDomPieceId(capture.piece.id));
          node.show('fade');
	  // update board model
	  this._board[capture.top][capture.left] = capture.piece;
	});
	// move the piece that was last to move, back
	var mover = this._board[transToRollBack.target.top][transToRollBack.target.left];
	// update model
        this._board[transToRollBack.target.top][transToRollBack.target.left] = '-';
        this._board[transToRollBack.origin.top][transToRollBack.origin.left] = mover;
	// move piece visually
        var pos_top = this.settings.square_size * transToRollBack.origin.top + this.settings.offsets.top;
        var pos_left = this.settings.square_size * transToRollBack.origin.left + this.settings.offsets.left;

        var node = $('#' + this.getDomPieceId(mover.id));
        node.animate({ position: 'absolute', top:pos_top, left:pos_left });
        this.showActiveSide();
      },

      transitionForward : function() {
	console.log('transition_current = ' + this.game.transition_current);
        if (this.game.transition_current >= this.game.transitions.length)
		return;

	var transToReplay = this.game.transitions[this.game.transition_current];

	// move the piece that was moving, again
	var mover = this._board[transToReplay.origin.top][transToReplay.origin.left];
	// update model
        this._board[transToReplay.origin.top][transToReplay.origin.left] = '-';
        this._board[transToReplay.target.top][transToReplay.target.left] = mover;
	// move piece visually
        var pos_top = this.settings.square_size * transToReplay.target.top + this.settings.offsets.top;
        var pos_left = this.settings.square_size * transToReplay.target.left + this.settings.offsets.left;

        var node = $('#' + this.getDomPieceId(mover.id));
        node.animate({ position: 'absolute', top:pos_top, left:pos_left });

	if (transToReplay.captured_pieces)
	// fade all captured pieces
        $.eachWithContext(this, transToReplay.captured_pieces, function(j, capture) {
          var node = $('#' + this.getDomPieceId(capture.piece.id));
          node.effect('fade');
	  // update board model
	  this._board[capture.top][capture.left] = '-';
	});
	if (transToReplay.winner) {
           if (transToReplay.winner == 'w') {
              $('#' + this.wrapper.id +' .piecea').effect('fade');
           } else {
              $('#' + this.wrapper.id +' .piecep').effect('fade');
           }
        }
	this.game.transition_current = this.game.transition_current + 1;
        this.showActiveSide();
      },

      annotation : function() {
      },

      boardElement : function() {
        return $(this.wrapper).find(this.settings.board_element_selector);
      },

      boardData : function() {
        return this._board;
      },

      setUpBoard : function(template) {
        this._board = this.createBoardDataFromTemplate(template);
      },

      createBoardDataFromTemplate : function(template) {
        var board = [];
        var bs = this.settings.board_size;
        $.each(template, function(j, row) {
          board[j] = [];
          $.each(row, function(k, val) {
            if (val != '-') {
              board[j][k] = { id: (k + 1) + (j * bs) , piece: template[j][k].toString() };
            } else {
              board[j][k] = '-';
            }
          });
        });

        return board;
      },

      activeSide : function() {
        if (this.game.transition_current % 2) {
            if (this.settings.rules.first_move = 'w')
                return 'b';
            else
                return 'w';
        } else
            return this.settings.rules.first_move;
      },

      showActiveSide : function() {
        var indicator = this.settings.active_side_indicator;
        if (indicator) {
          if (this.activeSide() == 'w') {
            indicator.removeClass('piecea');
            indicator.addClass('piecep');
          } else {
            indicator.removeClass('piecep');
            indicator.addClass('piecea');
          }
        }
      },

      writeBoard : function() {
        var board = this.boardElement();
        if (board.size() == 0) {
          $(this.wrapper).append('<div class="tablut-board"></div>');
          board = this.boardElement();
        }

        // hostile squares and the throne
        this.addHostileSquare(0, 0, 0);
        this.addHostileSquare(1, 0, this.settings.board_size - 1);
        this.addHostileSquare(2, this.settings.board_size - 1, 0);
        this.addHostileSquare(3, this.settings.board_size - 1, this.settings.board_size - 1);

        // board rulers
        for (var j = 0; j < this.settings.board_size; j++) {
          var first = (j == 0 ? " first" : "");
          board.append('<div id="' + this.getRulerId(j,"h") + '" class="horizontal' + first + '"></div>');
          $('#' + this.getRulerId(j,"h")).css({ position: 'absolute', top:this.settings.square_size*j, left:0, width: this.settings.square_size * this.settings.board_size + 'px' });
          board.append('<div id="' + this.getRulerId(j,"v") + '" class="vertical' + first + '"></div>');
          $('#' + this.getRulerId(j,"v")).css({ position: 'absolute', top:0, left:this.settings.square_size*j, height: this.settings.square_size * this.settings.board_size + 'px' });
        }

        board.append('<div id="' + this.getSpecialCellId(0, "throne") + '" class="throne"></div>');
        var middle = ~~(this.settings.board_size /2)*this.settings.square_size;
        $('#' + this.getSpecialCellId(0, "throne")).css({ position: 'absolute', top:middle, left:middle });

        $.eachWithContext(this, this.boardData(), function(j, row) {
          $.eachWithContext(this, row, function(k, piece) {
            var square = this.coord2Algebraic(j,k);
            if (piece != '-') this.addDomPiece(piece.id, piece.piece, square);
          });
        });
        board.droppable({ drop : this.draggingStopped });
        var size = this.settings.board_size*this.settings.square_size;
        board.css({ width:size+'px', height:size+'px' });
      },

      addHostileSquare : function(id, top, left) {
          this.boardElement().append('<div id="' + this.getSpecialCellId(id, "hostile") + '" class="hostile"></div>');
          $('#' + this.getSpecialCellId(id, "hostile")).css({ position: 'absolute', top:top*this.settings.square_size, left:left*this.settings.square_size });
      },
        
      getDomPieceId : function(id) {
        return this.wrapper.id + "_piece_" + id;
      },

      getRulerId : function(id, dir) {
        return this.wrapper.id + "_" + dir + "ruler_" + id;
      },

      getSpecialCellId : function(id, type) {
        return this.wrapper.id + "_" + type + "_" + id;
      },

      addDomPiece : function(id, piece, algebraic) {
        var square = this.algebraic2Coord(algebraic);
        if (this.game.board_direction < 0) {
          square[0] = this.settings.board_size - 1 - square[0];
          square[1] = this.settings.board_size - 1 - square[1];
        }

        var pos_top = this.settings.square_size * square[0] + this.settings.offsets.top;
        var pos_left = this.settings.square_size * square[1] + this.settings.offsets.left;

        this.boardElement().append('<div id="' + this.getDomPieceId(id) + '" class="piece' + piece + '"></div>');
        var node = $('#' + this.getDomPieceId(id));
        node.css({ position: 'absolute', top:pos_top, left:pos_left });
	node.draggable({ grid: [ this.settings.square_size, this.settings.square_size ], 
		containment: "parent", opacity: 0.7, helper: "clone",
                start: this.draggingStarted,
		drag: this.draggingInProgress
        });
        node.data('game', this);
      },

      draggingStarted : function(event, ui) {
        var game = $(this).data('game');
        game.game.move_origin.top = ~~($(this).position().top/game.settings.square_size);
        game.game.move_origin.left = ~~($(this).position().left/game.settings.square_size);  
      },

      draggingInProgress : function(event, ui) {
        var game = $(this).data('game');
        var target = game.screenToModel(ui.position);
        var active = game.belongsToActiveSide($(this));
        if (active && game.objectEquals(target, game.game.move_origin))
          return;
        if (active && game.isLegalMove($(this), ui.position)) {
          ui.helper.css({ border: "none" });
        } else {
          ui.helper.css({ border: "solid red" });
        }
      },

      belongsToActiveSide : function(node) {
        var active = this.activeSide();
        return active == 'b' && node.hasClass('piecea') || active == 'w' && (node.hasClass('piecep') || node.hasClass('piecek'));
      },

      screenToModel : function(pos) {
        return { top : pos.top/this.settings.square_size, left : pos.left/this.settings.square_size };
      },

      draggingStopped : function(event, ui) {
        var node = ui.draggable;
        var game = node.data('game');
        //console.log(game.game.active_color);

        if (game.belongsToActiveSide(node)) {
          if (game.isLegalMove(node, ui.position)) {
            node.css({ top:ui.position.top, left:ui.position.left });
            game.completeMove(node, ui.position);
          }
        }
      },

      isLegalMove : function(node, pos) {
        var target = this.screenToModel(pos);
        return this.freeWay(this.game.move_origin, target);
      },

      freeWay : function(origin, target) {
        //var board = this.boardData();
        //console.log(origin);
        //console.log(target);
        var piece = this._board[origin.top][origin.left];
        //console.log('piece=' + piece.piece);
        var minTop = Math.min(origin.top,target.top);
        var maxTop = Math.max(origin.top,target.top);
        var minLeft = Math.min(origin.left,target.left);
        var maxLeft = Math.max(origin.left,target.left);
        var piece2 = this._board[target.top][target.left];
        var middle = this.isMiddle(target.top, target.left);
        if ( piece2 == '-' && (origin.top == target.top || origin.left == target.left) ) {
          // check that non-King is not trying to move to hostile square
          if (piece.piece != 'k' && (this.isHostileCorner(target) || !this.settings.rules.soldiers_throne.enter && middle)) {
              return false;
          }
          // King may not re-enter vacated throne unless reenterable_throne setting is set to true
          if (piece.piece == 'k' && !this.settings.rules.reenterable_throne && middle) {
              return false;
          }
          // TODO optional base camp processing
          // check every cell between origin and target; whether we pass through throne can be actually
          // found from looking at origin and target, not calling isMiddle() in the loops
          for (var j = minTop + 1; j < maxTop; j++) {
            if (this._board[j][origin.left] != '-')
              return false;
            if (!this.settings.rules.soldiers_throne.pass && piece.piece != 'k') {
              if (this.isMiddle(j, origin.left))
                return false;
            }
          }
          for (var k = minLeft + 1; k < maxLeft; k++) {
            if (this._board[origin.top][k] != '-')
              return false;
            if (!this.settings.rules.soldiers_throne.pass && piece.piece != 'k') {
              if (this.isMiddle(origin.top, k))
                return false;
            }
          }
          return true;
        }
        return false;
      },

      isHostileCorner : function(target) {
        return (target.left + target.top + 2 == this.settings.board_size*2 || target.left + target.top == 0 || target.left == 0 && target.top == this.settings.board_size - 1 || target.left == this.settings.board_size - 1 && target.top == 0);
      },

      completeMove : function(node, pos) {
        // update board model
        //var board = this.boardData();
        var piece = this._board[this.game.move_origin.top][this.game.move_origin.left];
        //console.log(piece);
        this._board[this.game.move_origin.top][this.game.move_origin.left] = '-';
        var target = this.screenToModel(pos);
        this._board[target.top][target.left] = piece;
	// PGN-like transitions?
	this.game.transition = {origin:this.game.move_origin, target:target};
        // has King arrived?
        if (piece.piece == 'k' && this.isHostileCorner(target)) {
          $('#' + this.wrapper.id +' .piecea').effect('fade');
          this.game.transition = $.extend( {}, this.game.transition, {winner: this.activeSide()});
          this.appendTransition();
          return;
        }
        // is King blocked?
        if (piece.piece == 'a') {
          // is King above the current piece?
          if (target.top > 0 && this.isKing(target.top - 1, target.left)) {
            this.kingLostIfConfined(target.top - 1, target.left);
          }
          // is King below the current piece?
          if (target.top < this.settings.board_size - 1 && this.isKing(target.top + 1, target.left)) {
            this.kingLostIfConfined(target.top + 1, target.left);
          }
          // is King to the left from current piece?
          if (target.left > 0 && this.isKing(target.top, target.left - 1)) {
            this.kingLostIfConfined(target.top, target.left - 1);
          }
          // is King to the right from current piece?
          if (target.left < this.settings.board_size - 1 && this.isKing(target.top, target.left + 1)) {
            this.kingLostIfConfined(target.top, target.left + 1);
          }
        }

        if (this.isSoldier(piece.piece)) {
          if (target.top > 1 && this.isAnvil(target.top - 2, target.left)) {
            this.captureIfEnemyPresent(target.top - 1, target.left);
          }
          if (target.top < this.settings.board_size - 2 && this.isAnvil(target.top + 2, target.left)) {
            this.captureIfEnemyPresent(target.top + 1, target.left);
          }
          if (target.left > 1 && this.isAnvil(target.top, target.left - 2)) {
            this.captureIfEnemyPresent(target.top, target.left - 1);
          }
          if (target.left < this.settings.board_size - 2 && this.isAnvil(target.top, target.left + 2)) {
            this.captureIfEnemyPresent(target.top, target.left + 1);
          }
        }
        // did we repeat a position 3 times? or were 50 moves by each player made and no piece was captured?

        // is one of the sides confined?

        //this.debugBoard();
	this.appendTransition();
	// print debug information
        $.eachWithContext(this, this.game.transitions, function(j, move) {
          console.log('[' + j + '] = { origin.top: ' + move.origin.top + ', origin.left: ' + move.origin.left + ' } { target.top: ' + move.target.top + ', target.left: ' + move.target.left + ' }');
	  if (move.captured_pieces)
          $.eachWithContext(this, move.captured_pieces, function(k, val) {
            console.log('[' + j + ',' + k + '] = { top: ' + val.top + ', left: ' + val.left + ' }');
          });
        });
      },

      appendTransition : function() {
	// deep copy of current transition
	var newTrans = jQuery.extend(true, {}, this.game.transition);
	this.game.transitions.push(newTrans);
	this.game.transition_current = this.game.transition_current + 1;
        this.showActiveSide();
      },

      isKing : function(top, left) {
        return this._board[top][left].piece == 'k';
      },

      isConfiner : function(top, left) {
        var result = false;
        if (this.settings.rules.citadels.confinement.corner) {
          result = result || this.isHostileCorner({top:top,left:left});
        }
        if (this.settings.rules.citadels.confinement.throne) {
          result = result || this.isMiddle(top, left);
        }
        return result || this._board[top][left].piece == 'a';
      },

      isMiddle : function(top, left) {
          var middle = ~~(this.settings.board_size/2);
          return top == middle && left == middle;
      },

      kingLostIfConfined : function(top, left) {
        var confined = true;
        confined = confined && (top == 0 || this.isConfiner(top - 1, left));
        confined = confined && (top == this.settings.board_size - 1 || this.isConfiner(top + 1, left));
        confined = confined && (left == 0 || this.isConfiner(top, left - 1));
        confined = confined && (left == this.settings.board_size - 1 || this.isConfiner(top, left + 1));
        if (confined) {
          $('#' + this.wrapper.id +' .piecep').effect('fade');
          this.game.transition = $.extend( {}, this.game.transition, {winner: this.activeSide()});
	}
      },

      isSoldier : function(piece) {
        return piece != 'k' || this.settings.rules.king_in_captures.hammer;
      },

      isAnvil : function(top, left) {
        var result = false;
        var anvil = this._board[top][left];
        if (this.settings.rules.citadels.captures.corner) {
          result = result || this.isHostileCorner({top:top,left:left});
        }
        if (this.settings.rules.citadels.captures.throne) {
          // NB: only vacated throne can participate in capture, unless King is also anvil
          result = result || this.isMiddle(top, left) && anvil == '-';
        }
        //console.log('result1='+result + ' gac='+this.game.active_color);
        //console.log(anvil);
        if (this.activeSide() == 'w') {
          result = result || anvil.piece == 'p' || (anvil.piece == 'k' && this.settings.rules.king_in_captures.anvil);
        } else {
          result = result || anvil.piece == 'a';
        }
        //console.log('result2='+result);
        return result;
      },

      captureIfEnemyPresent : function(top, left) {
        var captive = this._board[top][left];
        var side = this.activeSide();
        if (side == 'w' && captive.piece == 'a' || side == 'b' && captive.piece == 'p') {
          this._board[top][left] = '-';
          var node = $('#' + this.getDomPieceId(captive.id));
          node.effect('fade');
	  if (!this.game.transition.captured_pieces)
		this.game.transition.captured_pieces = [];
          this.game.transition.captured_pieces.push({top:top, left:left, piece:captive});
        }
      },

      disableOrEnableDrag : function() {
        var white = this.activeSide() == 'w';
        $(".piecea").draggable( "option", "disabled", white );
        $(".piecep").draggable( "option", "disabled", !white );
        $(".piecek").draggable( "option", "disabled", !white );
      },

      debugBoard : function() {
        $.eachWithContext(this, this.boardData(), function(j, row) {
          $.eachWithContext(this, row, function(k, piece) {
            if (piece != '-')
            console.log('[' + j + ',' + k + '] = { id: ' + piece.id + ', piece: ' + piece.piece + ' }');
          });
        });
      },

      /* Utility Functions */
      algebraic2Coord : function(algebraic) {
        return [this.rank2Row(algebraic.substr(1, 1)), this.file2Col(algebraic.substr(0, 1))];
      },

      coord2Algebraic : function(row, col) {
        return this.col2File(col) + this.row2Rank(row);
      },

      rank2Row : function(rank) {
        return this.settings.board_size - parseInt(rank);
      },

      file2Col : function(file) {
        return file.charCodeAt(0) - ('a').charCodeAt(0);
      },

      row2Rank : function(row) {
        return (this.settings.board_size - row) + '';
      },

      col2File : function(col) {
        return String.fromCharCode( col + ('a').charCodeAt(0) );
      },

      flipVector : function(v) {
        return { x: (v.x * -1), y : (v.y * -1), limit : v.limit };
      },

      objectEquals : function(obj1, obj2) {
	    for (var i in obj1) {
		if (obj1.hasOwnProperty(i)) {
		    if (!obj2.hasOwnProperty(i)) return false;
		    if (obj1[i] != obj2[i]) return false;
		}
	    }
	    for (var i in obj2) {
		if (obj2.hasOwnProperty(i)) {
		    if (!obj1.hasOwnProperty(i)) return false;
		    if (obj1[i] != obj2[i]) return false;
		}
	    }
	    return true;
      },

      replaceNumberWithDashes : function(str) {
        var num_spaces = parseInt(str);
        var new_str = '';
        for (var i = 0; i < num_spaces; i++) { new_str += '-'; }
        return new_str;
      },

      parseFEN : function(fen) {
        // rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2
        var new_board = [];
        var fen_parts = fen.replace(/^\s*/, "").replace(/\s*$/, "").split(/\/|\s/);

        for (var j = 0; j < this.settings.board_size; j++) {
          new_board[j] = [];
          var row = fen_parts[j].replace(/\d/g, this.replaceNumberWithDashes);
          for (var k=0; k<this.settings.board_size; k++) {
            new_board[j][k] = row.substr(k, 1);
          }
        }
        return new_board;
      }
    }
  });
})(jQuery);
