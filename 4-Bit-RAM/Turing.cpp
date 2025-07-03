#include "comM.h"

size_t Turing::State_Symbol::hash() const
{
    if (state > symbol)
        return state * state + state + symbol;
    else
        return symbol * symbol + state;
}

Turing::State_Symbol_Movement& Turing::Rules::operator[](Turing::State_Symbol& key)
{
    if (auto search = map.find(key); search != map.end()) return (*search).second;
    throw std::runtime_error("Missing Rule");
}

void Turing::Rules::add(State_Symbol State_symbolKey, State_Symbol_Movement State_symbol_MoveValue)
{
    map[State_symbolKey] = State_symbol_MoveValue;
}

void Turing::Rules::add(std::vector<size_t>& State_Keys, size_t Symbol_key, State_Symbol_Movement State_symbol_MoveValue)
{
    for (size_t State_Key : State_Keys)
        map[{State_Key, Symbol_key}] = State_symbol_MoveValue;
}

void Turing::Rules::add(std::vector<size_t>& State_Keys, size_t Symbol_key, std::vector<size_t>& State_Values, size_t Symbol_Value, size_t Move_Value)
{
    for (int i{}; i < std::ssize(State_Keys); ++i)
        map[{State_Keys[i], Symbol_key}] = { State_Values[i],Symbol_Value,Move_Value };
}



Turing::Turing_Machine::Turing_Machine(RuleSet& rules, size_t start_state)
    :
    transformation_rules{ rules },
    start_state{ start_state }
{
}

void Turing::Turing_Machine::boot_up(std::vector<size_t>& input)
{
    input_sequence = &input;
    tape = std::vector<size_t>(std::ssize(*input_sequence));

    reset();

    //The input is inserted into the tape from the leftmost cell.
    for (int i{}; i < std::ssize(*input_sequence); i++)
    {
        tape[i] = (*input_sequence)[i];
    }
}

void Turing::Turing_Machine::run()
{
    while (!check_done()) step();
}

void Turing::Turing_Machine::reset()
{
    position = 0;
    std::fill(tape.begin(), tape.end(), static_cast<size_t>(0));
    state = start_state;
}

void Turing::Turing_Machine::step()
{
    SS current_step{ state,tape[position] };
    SSM next_step{ transformation_rules[current_step] };

    state = next_step.state;
    tape[position] = next_step.symbol;

    switch (static_cast<Movement>(next_step.move))
    {
    case(Movement::STAY):
        break;
    case(Movement::LEFT):
        --position;
        assert((position >= 0) && "Head attempted to move out of tape : left");
        break;
    case(Movement::RIGHT):
        ++position;
        if (position >= std::ssize(tape)) tape.push_back(BLANK);
        break;
    };

}

bool Turing::Turing_Machine::check_done()
{
    switch (state)
    {
    case HALT:
    case ACCEPT:
    case REJECT:
        return true;
    default:
        return false;
    }
}