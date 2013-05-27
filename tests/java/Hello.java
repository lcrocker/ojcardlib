/* OneJoker card library <http://lcrocker.github.io/onejoker/cardlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * "Hello World" app.
 */
import com.onejoker.cardlib.*;

public class Hello {
    static {
        System.loadLibrary("ojcard");
    }
    public static void main(String[] args) {
        CardList deck = new CardList(52), hand = new CardList(5);
        deck.fill();
        deck.shuffle();

        for (int i = 0; i < 5; ++i) hand.add(deck.pop());
        System.out.println(hand);
    }
}
