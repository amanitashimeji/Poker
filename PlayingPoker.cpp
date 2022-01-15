
#include <iostream>
#include <vector>
#include <optional>
#include <array>
#include <random>
#include <memory>
#include <algorithm>


namespace /*Cardクラス*/ {
class Card{
    const int id_;    
    const int suit_;
    const int value_;

    public:

    Card():Card(-1,-1,-1){}

    Card(const int id,const int suit,const int num)
    :id_(id),suit_(suit),value_(num)
    {}

    Card(const Card &cd)
    :id_(cd.id_),suit_(cd.suit_),value_(cd.value_)
    {}

    Card(Card&& m) noexcept
    :id_(std::move(m.id_)),suit_(std::move(m.suit_)),value_(std::move(m.value_))
    {}

    ~Card()=default; 

    void PrintCard()const{
        std::cout << SuitEmoji() << Value();
    }
    std::pair<std::string,std::string> Mark()const;

    std::string SuitEmoji()const;

    std::string Value()const;

    
};

std::string Card::Value() const{
    if (value_ > 1 && value_ < 10){
        std::string result = std::to_string(value_);
        return result;
    }
    switch (value_){
    case 1:     return "A";
    case 10:    return "T";
    case 11:    return "J";
    case 12:    return "Q";
    case 13:    return "K";
    default:    return " ";
    }
}

std::string Card::SuitEmoji() const{
    switch (suit_){
    case 1:     return "♠";
    case 2:     return "♣";
    case 3:     return "♥";
    case 4:     return "♦";
    default:    return " ";
    }
}

std::pair<std::string, std::string> Card::Mark() const{
    return {SuitEmoji(), Value()};
}
}

namespace /*Deckクラス*/{
class Deck{

    std::vector<Card> deck;
    std::vector<int> list;

public:
    Deck(){
        int count = 1;
        for (int i = 1; i < 5; ++i){
            for (int j = 1; j < 14; ++j){
                int id = count;
                int suit = i;
                int value = j;
                deck.emplace_back(id, suit, value);
                list.emplace_back(id);
                count ++;

            }
        }
    }
    std::vector<int>& shuffle(std::mt19937& engine){
        std::shuffle(list.begin(),list.end(),engine);
        return list;
    }

    std::vector<Card>& DeckReference(){
        return deck;
    }
    
    void Ref(const int id){
        const int index=id-1;
        deck.at(index).PrintCard();
    }

};
}

namespace /*Playerクラス*/{
class Player{

    int id_;
    std::array<std::optional<int>,5> hands_;

    size_t cardcount_;

    public:

    Player():Player(-1){}

    Player(const int id)
        :id_(id),
        hands_{std::nullopt,std::nullopt,std::nullopt,std::nullopt,std::nullopt},
        cardcount_(0){}
    
    ~Player()=default;

    void AddCard(const int card_id){
        hands_.at(cardcount_)=card_id;
        cardcount_++;
    }

    bool fullOfHands(){
        if(cardcount_==5)return true;
        return false;
    }

    std::array<std::optional<int>,5>& OpenHands(){
        return hands_;
    }

    void hands_size()const{
        std::cout << cardcount_;
    }

    void Init(){
        for(auto& item:hands_){
            item=std::nullopt;
        }
        cardcount_=0;
    }

};


}

namespace /*Pokerクラス*/{
enum class State{
    Opening,
    PreFrop,
    PostFrop,
    Turn,
    River,
    ShowDown,
    Ending,
};
 

class Poker{
    State st;
    std::string userinput;
    bool is_continue;
    std::unique_ptr<Deck> deck;
    std::vector<int> list;
    Player BOARD;
    std::mt19937& mt;
    std::array<Player,6> players;

    public:
    Poker(std::mt19937& mt)
    :st(State::Opening),userinput(""),is_continue(true),
     deck(std::make_unique<Deck>()),
     BOARD(Player(-1)),
     mt(mt){
        std::cout <<"テキサスホールデムへようこそ\n";
    }
    
    bool YesOrNo(const std::string& st){
        if(st =="Y"||st=="y"||st=="Yes"||st=="YES"||st=="yes")return true;
        return false;
    }
    void Update(){
        switch(st){
        case State::Opening   : Opening()   ; return;
        case State::PreFrop   : PreFrop()   ; return;
        case State::PostFrop  : PostFrop()  ; return;
        case State::Turn      : Turn()      ; return;
        case State::River     : River()     ; return;
        case State::ShowDown  : ShowDown()  ; return;
        case State::Ending    : Ending()    ; return;        
        }
    }
    bool isContinue()const{
        return is_continue;
    }

    private:

    void Opening(){
        std::cout <<"ゲームを開始しますか？[y/n]:";
        std::cin >> userinput;
        if(YesOrNo(userinput)){
            st =State::PreFrop;
            return;
        }
        st =State::Ending;
        return;
    }

    void PreFrop(){
        std::cout <<"プリフロップ:";
        InitPlayers();
        list =deck->shuffle(mt);
        SendCardForPlayers();
        SendCardForPlayers();

        const auto user =players.at(0).OpenHands();
        std::cout <<"あなたの手札は";
        PrintCardofId(user);

        std::cout <<"\n 続けますか?[y/n]";
        std::cin >>userinput;

        YesOrNo(userinput) ? st=State::PostFrop : st=State::Opening ;

        return;
    }

    void PostFrop(){
        std::cout <<"フロップ:\n";
        MakingBOARD(3);
        std::cout <<"ボードは";
        const auto board =BOARD.OpenHands();
        PrintCardofId(board);
        
        const auto user =players.at(0).OpenHands();
        std::cout <<"\nあなたの手札は";
        PrintCardofId(user);
        
        std::cout <<"\n 続けますか?[y/n]";
        std::cin >>userinput;
        YesOrNo(userinput) ? st=State::Turn : st=State::Opening ;

        return;
    }

    void Turn(){
        std::cout <<"ターン:";
        std::cout <<"ボードは";

        SendCardForBOARD();
        const auto board =BOARD.OpenHands();
        PrintCardofId(board);
        
        const auto user =players.at(0).OpenHands();
        std::cout <<"\nあなたの手札は";
        PrintCardofId(user);
        
        std::cout <<"\n 続けますか?[y/n]";
        std::cin >>userinput;
        YesOrNo(userinput) ? st=State::River : st=State::Opening ;
        return;
    }

    void River(){
        std::cout <<"リバー:";

        std::cout <<"ボードは";
        SendCardForBOARD();
        const auto board =BOARD.OpenHands();
        PrintCardofId(board);
        
        const auto user =players.at(0).OpenHands();
        std::cout <<"\nあなたの手札は";
        PrintCardofId(user);
        
        std::cout <<"\n 続けますか?[y/n]";
        std::cin >>userinput;
        YesOrNo(userinput) ? st=State::ShowDown : st=State::Opening ;
        st=State::ShowDown;
        return;
    }

    void ShowDown(){
        std::cout <<"ショーダウン:";
        std::cout <<"ボードは";
        const auto board =BOARD.OpenHands();
        PrintCardofId(board);

        const auto user =players.at(0).OpenHands();
        std::cout <<"\nあなたの手札は";
        PrintCardofId(user);

        std::cout <<"\n";

        ShowDownPlayers();

        st=State::Ending;
        return;
    }

    void Ending(){
        InitPlayers();
        std::cout <<"ゲームを終了しますか？[y/n]:";
 
        std::cin >> userinput;
        if(YesOrNo(userinput)){
            is_continue=false;
            return;
        }
        st =State::Opening;
        return;        
    }


    void InitPlayers(){
    for(int count =0;auto &player:players){
        player = Player(count);
        count++;
    }
    BOARD=Player(-1);
    }

    void PrintCardofId(const std::array<std::optional<int>,5>& hand){
        for(const auto& card_id:hand){
            if(card_id.has_value()){

                int id =card_id.value();
                deck->Ref(id);
            }
        }

    }
    void ShowDownPlayers(){
        for(auto i=1;i<players.size();i++){
            std::cout <<"NPC" <<i <<":";
            const auto npc =players.at(i).OpenHands();
            PrintCardofId(npc);

            std::cout <<"\n";
        }

    }
    void SendCardForPlayers(){
        for(auto count =0;auto player:players){
            players.at(count).AddCard(list.back());

            list.pop_back();

            count++;
            
        }
    }
    void MakingBOARD(const int count){
        for(auto i=0;i<count;i++){
        SendCardForBOARD();
        }
    }
    void SendCardForBOARD(){
        BOARD.AddCard(list.back());
        list.pop_back();
    }

};
}

int main(){

    std::random_device seed;
    std::mt19937 mt(seed());
    //乱数生成器はデッキクラスに持っていてほしいなあ……

    Poker poker(mt);

    while(poker.isContinue())
    {
        poker.Update();
    }

    std::cout <<"お疲れさまでした\n";
}
