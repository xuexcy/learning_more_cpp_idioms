/**
########################################################################
#
# Copyright (c) 2025 xx.com, Inc. All Rights Reserved
#
########################################################################
# Author : xuechengyun
# E-mail : xuechengyun@gmail.com
# Date   : 2025/01/04 21:59:07
# Desc   :
########################################################################
构造函数只能通过参数区分，如果有很多构造函数，那么调用的时候就很难一眼看出来调用的是哪个构造函数。

@solution:
将构造函数设置为 private，通过 static class function 来构造实例，给这些 static 函数起一个 meaningful
的名字
*/

class Game {
public:
  static Game createSinglePlayerGame() { return Game(0); }  // named constructor
  static Game createMultiPlayerGame() { return Game(1); }   // named constructor
protected:
  Game(int game_type): game_type_(game_type) {};
private:
  int game_type_;
};  // class Game

int main() {
  Game g1 = Game::createSinglePlayerGame();  // Using named constructor
  Game g2 = Game::createMultiPlayerGame();   // Using named constructor
  // Game g3 = Game(1); // multiplayer game; without named constructor (does not compile)
  return 0;
}
