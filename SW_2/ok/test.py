from sw2_1_Classi import *
from sw2_1_Collector import *

if __name__ == "__main__":
    d1 = Device("D1", ["nulla", "niente"], ["nothing", "none"])
    d2 = Device("D2", ["nulla", "niente"], ["nothing", "none"])
    d3 = Device("D3", ["nulla", "niente"], ["nothing", "none"])
    d4 = Device("D3", ["nulla", "niente"], ["nothing", "none"])


    u1 = User("U1", "pippo", "baudo", "pippobaudo@gmail.com")
    u2 = User("U2", "benito", "mussolini", "fascio_appeso@foibe.it")
    u3 = User("U3", "gennaro", "'o mort vivent", "gennarothebest@sfaccimm.it")
    u4 = User("U4", "direttore", "coletta", "come@cazzo.va")

    s1 = Service("S1", "Pompini gratis", "blowjobs for free", ["ceh", "figa"])
    s2 = Service("S2", "Acquapark", "divertimento e spaccio", ["matrimoni gay", "e altre cose inutili"])
    s3 = Service("S3", "Ristorante", "Cucina tipica africana", [])

    c = Collector()
    c.add(d1)
    c.add(d2)
    c.add(d3)
    c.add(d4)
    l = c.listItems()
    print(l)

